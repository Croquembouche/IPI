#include "ipi/api/pc5_ipi_adapter.hpp"
#include "ipi/v2x/j2735_ipi_regional_codec.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>
#include <vector>

extern "C" {
#define class class_field
#include <v2x_api.h>
#undef class
int v2x_packet_data_send(char* buffer, int len, int msg_id);
}

namespace {

constexpr int kMocarCustomMessageId = 0x1b;

enum class Role { Initiator, Responder, Both };

struct Args {
  Role role{Role::Initiator};
  std::string nodeId{"node-a"};
  std::string peerId{"node-b"};
  std::string intersectionId{"intersection-1"};
  std::string sessionId{"pc5-session-1"};
  std::uint32_t count{20};
  std::uint32_t intervalMs{100};
  std::uint32_t timeoutMs{1000};
  std::size_t bodyBytes{128};
  std::size_t maxPacketBytes{ipi::api::kPc5IpiOperationalMaxBytes};
};

Args g_args;
std::mutex g_mutex;
std::condition_variable g_condition;
std::optional<ipi::api::Pc5IpiEnvelope> g_outstanding;
std::optional<ipi::api::Pc5IpiResponse> g_response;
std::unordered_set<std::string> g_completed_response_ids;

std::uint64_t parse_u64(std::string_view label, const char* value) {
  char* end = nullptr;
  const auto parsed = std::strtoull(value, &end, 10);
  if (end == value || *end != '\0') {
    throw std::invalid_argument("invalid " + std::string(label));
  }
  return parsed;
}

Role parse_role(std::string_view value) {
  if (value == "initiator") return Role::Initiator;
  if (value == "responder") return Role::Responder;
  if (value == "both") return Role::Both;
  throw std::invalid_argument("--role must be initiator, responder, or both");
}

Args parse_args(int argc, char** argv) {
  Args args;
  for (int index = 1; index < argc; ++index) {
    const std::string_view argument(argv[index]);
    auto value = [&](const char* label) -> const char* {
      if (index + 1 >= argc) throw std::invalid_argument(std::string("missing ") + label);
      return argv[++index];
    };
    if (argument == "--help" || argument == "-h") {
      std::cout
          << "Usage: " << argv[0] << " --role initiator|responder|both [options]\n"
          << "  --node-id ID --peer-id ID --intersection-id ID --session-id ID\n"
          << "  --count N --interval-ms N --timeout-ms N --body-bytes N\n"
          << "  --max-packet-bytes N (default 2048, deployed hard ceiling 4080)\n\n"
          << "This sample carries a J2735 TestMessage00 IPI regional UPER value inside\n"
          << "the versioned IP5X integration envelope over Mocar's custom channel.\n";
      std::exit(0);
    } else if (argument == "--role") {
      args.role = parse_role(value("--role"));
    } else if (argument == "--node-id") {
      args.nodeId = value("--node-id");
    } else if (argument == "--peer-id") {
      args.peerId = value("--peer-id");
    } else if (argument == "--intersection-id") {
      args.intersectionId = value("--intersection-id");
    } else if (argument == "--session-id") {
      args.sessionId = value("--session-id");
    } else if (argument == "--count") {
      args.count = static_cast<std::uint32_t>(parse_u64("--count", value("--count")));
    } else if (argument == "--interval-ms") {
      args.intervalMs = static_cast<std::uint32_t>(
          parse_u64("--interval-ms", value("--interval-ms")));
    } else if (argument == "--timeout-ms") {
      args.timeoutMs = static_cast<std::uint32_t>(
          parse_u64("--timeout-ms", value("--timeout-ms")));
    } else if (argument == "--body-bytes") {
      args.bodyBytes = static_cast<std::size_t>(
          parse_u64("--body-bytes", value("--body-bytes")));
    } else if (argument == "--max-packet-bytes") {
      args.maxPacketBytes = static_cast<std::size_t>(
          parse_u64("--max-packet-bytes", value("--max-packet-bytes")));
    } else {
      throw std::invalid_argument("unknown argument: " + std::string(argument));
    }
  }
  if (args.nodeId.empty() || args.peerId.empty() || args.intersectionId.empty() ||
      args.sessionId.empty() || args.count == 0 || args.timeoutMs == 0 ||
      args.nodeId.size() > 16 || args.bodyBytes > 2048 ||
      args.maxPacketBytes == 0 ||
      args.maxPacketBytes > ipi::api::kMocarDeployedApplicationHardMaxBytes) {
    throw std::invalid_argument("invalid or empty PC5 sample option");
  }
  return args;
}

ipi::api::Pc5IpiCodecLimits limits() {
  return ipi::api::Pc5IpiCodecLimits{g_args.maxPacketBytes};
}

ipi::v2x::J2735IpiRegionalCodec regional_codec() {
  ipi::v2x::J2735IpiRegionalProfile profile;
  profile.maxFrameBytes = g_args.maxPacketBytes;
  return ipi::v2x::J2735IpiRegionalCodec(profile);
}

ipi::SessionId session_bytes() {
  ipi::SessionId value{};
  for (std::size_t index = 0; index < g_args.sessionId.size(); ++index) {
    value[index % value.size()] ^= static_cast<std::uint8_t>(g_args.sessionId[index]);
  }
  return value;
}

int send_packet(const std::vector<std::uint8_t>& packet) {
  if (packet.empty() || packet.size() > g_args.maxPacketBytes ||
      packet.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    return -1;
  }
  return v2x_packet_data_send(
      reinterpret_cast<char*>(const_cast<std::uint8_t*>(packet.data())),
      static_cast<int>(packet.size()), kMocarCustomMessageId);
}

ipi::api::Pc5IpiEnvelope make_request(std::uint64_t sequence) {
  const auto now = std::chrono::system_clock::now();
  ipi::api::Pc5IpiEnvelope request;
  auto& metadata = request.message.metadata;
  metadata.messageId = g_args.nodeId + "-message-" + std::to_string(sequence);
  metadata.correlationId = g_args.nodeId + "-request-" + std::to_string(sequence);
  metadata.sessionId = g_args.sessionId;
  metadata.sequence = sequence;
  metadata.sentAt = now;
  metadata.expiresAt = now + std::chrono::milliseconds(g_args.timeoutMs);
  metadata.intersectionId = g_args.intersectionId;
  metadata.transport = ipi::api::TransportType::C_V2X;
  metadata.source.type = ipi::api::SourceType::PCAV;
  metadata.source.id = g_args.nodeId;
  ipi::CooperativeServiceMessage service;
  service.sessionId = session_bytes();
  service.vehicleId.assign(g_args.nodeId.begin(), g_args.nodeId.end());
  service.serviceClass = ipi::ServiceClass::GuidedPlanning;
  service.guidanceStatus = ipi::GuidanceStatus::Request;
  service.requestedHorizonMs = static_cast<std::uint16_t>(
      std::min<std::uint32_t>(g_args.timeoutMs, 60000U));
  service.offloadTaskId = metadata.correlationId;
  service.offloadPayload = std::vector<std::uint8_t>(g_args.bodyBytes);
  for (std::size_t index = 0; index < service.offloadPayload->size(); ++index) {
    (*service.offloadPayload)[index] = static_cast<std::uint8_t>((sequence + index) & 0xffU);
  }

  request.message.data.type = ipi::api::J2735MessageType::IPI_COOPERATIVE_SERVICE;
  request.message.data.encoding = ipi::api::J2735Encoding::UPER;
  request.message.data.frameCounter = static_cast<std::uint32_t>(sequence);
  request.message.data.payload = regional_codec().encode_message_frame(service);
  return request;
}

void respond(const ipi::api::Pc5IpiEnvelope& request) {
  auto completed = regional_codec().decode_message_frame(request.message.data.payload);
  completed.guidanceStatus = ipi::GuidanceStatus::Complete;

  ipi::api::Pc5IpiResponse response;
  response.responseId = g_args.nodeId + "-response-" +
                        std::to_string(*request.message.metadata.sequence);
  response.requestMessageId = request.message.metadata.messageId;
  response.correlationId = *request.message.metadata.correlationId;
  response.sessionId = request.message.metadata.sessionId;
  response.sequence = *request.message.metadata.sequence;
  response.expiresAt = std::chrono::system_clock::now() +
                       std::chrono::milliseconds(g_args.timeoutMs);
  response.accepted = true;
  response.failureCode = ipi::api::FailureCode::NONE;
  response.detail = "processed-by-" + g_args.nodeId;
  response.result = request.message.data;
  response.result.payload = regional_codec().encode_message_frame(completed);
  const auto encoded = ipi::api::encode_pc5_ipi_response(response, limits());
  const auto result = send_packet(encoded);
  std::cerr << "pc5_response_tx,response_id=" << response.responseId
            << ",sequence=" << response.sequence << ",packet_bytes=" << encoded.size()
            << ",result=" << result << '\n';
}

void receive_callback(char* buffer, int length) {
  if (buffer == nullptr || length <= 0 ||
      static_cast<std::size_t>(length) > g_args.maxPacketBytes) {
    return;
  }
  const auto first = reinterpret_cast<const std::uint8_t*>(buffer);
  std::vector<std::uint8_t> packet(first, first + length);
  try {
    const auto request = ipi::api::decode_pc5_ipi_envelope(packet, limits());
    if (request.message.data.type !=
            ipi::api::J2735MessageType::IPI_COOPERATIVE_SERVICE ||
        request.message.data.encoding != ipi::api::J2735Encoding::UPER) {
      throw std::runtime_error("PC5 request is not an IPI J2735 regional UPER value");
    }
    (void)regional_codec().decode_message_frame(request.message.data.payload);
    if ((g_args.role == Role::Responder || g_args.role == Role::Both) &&
        request.message.metadata.source.id != g_args.nodeId) {
      respond(request);
    }
    return;
  } catch (const std::exception&) {
  }

  try {
    const auto response = ipi::api::decode_pc5_ipi_response(packet, limits());
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_outstanding) return;
    const auto validation = ipi::api::validate_pc5_ipi_response(
        *g_outstanding, response, g_completed_response_ids, false,
        std::chrono::system_clock::now());
    if (!validation.matched()) {
      std::cerr << "pc5_response_rejected,disposition="
                << ipi::api::to_string(validation.disposition)
                << ",detail=" << validation.detail << '\n';
      return;
    }
    const auto completed = regional_codec().decode_message_frame(response.result.payload);
    if (completed.guidanceStatus != ipi::GuidanceStatus::Complete ||
        completed.sessionId != session_bytes() ||
        !completed.offloadTaskId ||
        *completed.offloadTaskId != response.correlationId) {
      throw std::runtime_error("PC5 response IPI result identity or status mismatch");
    }
    g_completed_response_ids.insert(response.responseId);
    g_response = response;
    g_condition.notify_all();
  } catch (const std::exception& error) {
    std::cerr << "pc5_packet_rejected,detail=" << error.what() << '\n';
  }
}

void run_initiator() {
  for (std::uint64_t sequence = 1; sequence <= g_args.count; ++sequence) {
    auto request = make_request(sequence);
    const auto encoded = ipi::api::encode_pc5_ipi_envelope(request, limits());
    const auto started = std::chrono::steady_clock::now();
    {
      std::lock_guard<std::mutex> lock(g_mutex);
      g_outstanding = request;
      g_response.reset();
    }
    const auto sendResult = send_packet(encoded);
    if (sendResult != 0) {
      std::cerr << "pc5_request_tx_failed,sequence=" << sequence
                << ",result=" << sendResult << '\n';
    } else {
      std::unique_lock<std::mutex> lock(g_mutex);
      const bool received = g_condition.wait_for(
          lock, std::chrono::milliseconds(g_args.timeoutMs),
          [] { return g_response.has_value(); });
      const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::steady_clock::now() - started).count();
      std::cerr << "pc5_request_complete,sequence=" << sequence
                << ",packet_bytes=" << encoded.size()
                << ",matched=" << (received ? "true" : "false")
                << ",rtt_ns=" << (received ? elapsed : -1) << '\n';
    }
    {
      std::lock_guard<std::mutex> lock(g_mutex);
      g_outstanding.reset();
      g_response.reset();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(g_args.intervalMs));
  }
}

}  // namespace

int main(int argc, char** argv) {
  try {
    g_args = parse_args(argc, argv);
    if (mde_v2x_init(0) != 0) {
      std::cerr << "mde_v2x_init failed\n";
      return 1;
    }
    if (mde_v2x_custom_recv_handle_register(receive_callback) != 0) {
      std::cerr << "mde_v2x_custom_recv_handle_register failed\n";
      return 1;
    }
    std::cerr << "ipi_pc5_ready,node_id=" << g_args.nodeId
              << ",max_packet_bytes=" << g_args.maxPacketBytes
              << ",body_bytes=" << g_args.bodyBytes << '\n';
    if (g_args.role == Role::Initiator || g_args.role == Role::Both) {
      run_initiator();
      return 0;
    }
    while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
  } catch (const std::exception& error) {
    std::cerr << "ipi_pc5_exchange: " << error.what() << '\n';
    return 2;
  }
}
