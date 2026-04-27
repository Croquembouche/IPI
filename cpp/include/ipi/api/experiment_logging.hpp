#pragma once

#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace ipi::api {

struct ExperimentContext {
    std::string runId{"run-local"};
    std::string conditionId{"condition-default"};
    std::string conditionLabel{};
    std::string requestIdBase{"request"};
    std::string avId{};
    std::string obuId{};
    std::string rsuId{};
    std::string networkLoadLevel{"idle"};
    std::string qosProfile{"default"};
    std::string mobilityState{"stationary"};
    std::string clockSyncState{"unspecified"};
    bool serviceSuccess{true};
    std::string vehicleOutcomeName{};
    std::string vehicleOutcomeValue{};
    std::string vehicleOutcomeUnit{};
};

struct ExperimentLogRecord {
    std::string emitterRole{};
    std::uint64_t emitTimeNs{0};
    std::string runId{};
    std::string conditionId{};
    std::string conditionLabel{};
    std::string serviceType{};
    std::string transport{};
    std::string avId{};
    std::string obuId{};
    std::string rsuId{};
    std::string requestId{};
    std::string sessionId{};
    std::string intersectionId{};
    std::string sourceId{};
    std::uint64_t sequence{0};
    std::string networkLoadLevel{};
    std::string qosProfile{};
    std::string mobilityState{};
    std::string clockSyncState{};
    bool accepted{false};
    bool serviceSuccess{false};
    std::string vehicleOutcomeName{};
    std::string vehicleOutcomeValue{};
    std::string vehicleOutcomeUnit{};
    std::string frameType{};
    std::uint32_t payloadBytes{0};
    std::optional<std::uint64_t> clientSendTimeNs{};
    std::optional<std::uint64_t> serverReceiveTimeNs{};
    std::optional<std::uint64_t> serverSendTimeNs{};
    std::optional<std::uint64_t> clientReceiveTimeNs{};
    std::optional<double> roundTripMs{};
    std::optional<double> uplinkMs{};
    std::optional<double> serverMs{};
    std::optional<double> downlinkMs{};
    std::string detail{};
};

inline bool parse_experiment_bool(std::string_view value) {
    if (value == "1" || value == "true" || value == "yes" || value == "on") {
        return true;
    }
    if (value == "0" || value == "false" || value == "no" || value == "off") {
        return false;
    }
    throw std::invalid_argument("invalid boolean value: " + std::string(value));
}

inline std::string csv_escape(const std::string& value) {
    if (value.find_first_of(",\"\n\r") == std::string::npos) {
        return value;
    }
    std::string escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back('"');
    for (const char c : value) {
        if (c == '"') {
            escaped.push_back('"');
        }
        escaped.push_back(c);
    }
    escaped.push_back('"');
    return escaped;
}

inline std::string bool_text(bool value) {
    return value ? "true" : "false";
}

inline std::string format_optional_uint64(const std::optional<std::uint64_t>& value) {
    return value ? std::to_string(*value) : std::string{};
}

inline std::string format_optional_double(const std::optional<double>& value) {
    if (!value) {
        return {};
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << *value;
    return oss.str();
}

inline std::string experiment_log_csv_header() {
    return "emitter_role,emit_time_ns,run_id,condition_id,condition_label,service_type,transport,av_id,obu_id,rsu_id,"
           "request_id,session_id,intersection_id,source_id,sequence,network_load_level,qos_profile,mobility_state,"
           "clock_sync_state,accepted,service_success,vehicle_outcome_name,vehicle_outcome_value,vehicle_outcome_unit,"
           "frame_type,payload_bytes,client_send_ns,server_receive_ns,server_send_ns,client_receive_ns,rtt_ms,"
           "uplink_ms,server_ms,downlink_ms,detail";
}

inline std::string experiment_log_to_csv(const ExperimentLogRecord& record) {
    std::ostringstream oss;
    oss << csv_escape(record.emitterRole) << ','
        << record.emitTimeNs << ','
        << csv_escape(record.runId) << ','
        << csv_escape(record.conditionId) << ','
        << csv_escape(record.conditionLabel) << ','
        << csv_escape(record.serviceType) << ','
        << csv_escape(record.transport) << ','
        << csv_escape(record.avId) << ','
        << csv_escape(record.obuId) << ','
        << csv_escape(record.rsuId) << ','
        << csv_escape(record.requestId) << ','
        << csv_escape(record.sessionId) << ','
        << csv_escape(record.intersectionId) << ','
        << csv_escape(record.sourceId) << ','
        << record.sequence << ','
        << csv_escape(record.networkLoadLevel) << ','
        << csv_escape(record.qosProfile) << ','
        << csv_escape(record.mobilityState) << ','
        << csv_escape(record.clockSyncState) << ','
        << bool_text(record.accepted) << ','
        << bool_text(record.serviceSuccess) << ','
        << csv_escape(record.vehicleOutcomeName) << ','
        << csv_escape(record.vehicleOutcomeValue) << ','
        << csv_escape(record.vehicleOutcomeUnit) << ','
        << csv_escape(record.frameType) << ','
        << record.payloadBytes << ','
        << format_optional_uint64(record.clientSendTimeNs) << ','
        << format_optional_uint64(record.serverReceiveTimeNs) << ','
        << format_optional_uint64(record.serverSendTimeNs) << ','
        << format_optional_uint64(record.clientReceiveTimeNs) << ','
        << format_optional_double(record.roundTripMs) << ','
        << format_optional_double(record.uplinkMs) << ','
        << format_optional_double(record.serverMs) << ','
        << format_optional_double(record.downlinkMs) << ','
        << csv_escape(record.detail);
    return oss.str();
}

inline std::string experiment_log_to_text(const ExperimentLogRecord& record) {
    std::ostringstream oss;
    oss << "role=" << record.emitterRole
        << " run=" << record.runId
        << " condition=" << record.conditionId;
    if (!record.conditionLabel.empty()) {
        oss << " label=" << record.conditionLabel;
    }
    oss << " service=" << record.serviceType
        << " transport=" << record.transport
        << " request=" << record.requestId;
    if (!record.sessionId.empty()) {
        oss << " session=" << record.sessionId;
    }
    if (!record.intersectionId.empty()) {
        oss << " intersection=" << record.intersectionId;
    }
    if (!record.sourceId.empty()) {
        oss << " source=" << record.sourceId;
    }
    if (!record.avId.empty()) {
        oss << " av=" << record.avId;
    }
    if (!record.obuId.empty()) {
        oss << " obu=" << record.obuId;
    }
    if (!record.rsuId.empty()) {
        oss << " rsu=" << record.rsuId;
    }
    if (record.sequence != 0) {
        oss << " seq=" << record.sequence;
    }
    oss << " load=" << record.networkLoadLevel
        << " qos=" << record.qosProfile
        << " mobility=" << record.mobilityState
        << " sync=" << record.clockSyncState
        << " accepted=" << bool_text(record.accepted)
        << " service_success=" << bool_text(record.serviceSuccess)
        << " bytes=" << record.payloadBytes;
    if (!record.frameType.empty()) {
        oss << " frame=" << record.frameType;
    }
    if (record.roundTripMs) {
        oss << " rtt_ms=" << format_optional_double(record.roundTripMs);
    }
    if (record.uplinkMs) {
        oss << " uplink_ms=" << format_optional_double(record.uplinkMs);
    }
    if (record.serverMs) {
        oss << " server_ms=" << format_optional_double(record.serverMs);
    }
    if (record.downlinkMs) {
        oss << " downlink_ms=" << format_optional_double(record.downlinkMs);
    }
    if (!record.vehicleOutcomeName.empty()) {
        oss << " vehicle_outcome=" << record.vehicleOutcomeName;
        if (!record.vehicleOutcomeValue.empty()) {
            oss << ':' << record.vehicleOutcomeValue;
            if (!record.vehicleOutcomeUnit.empty()) {
                oss << record.vehicleOutcomeUnit;
            }
        }
    }
    if (!record.detail.empty()) {
        oss << " detail=\"" << record.detail << '"';
    }
    return oss.str();
}

inline std::string default_condition_label(std::string_view transport, std::string_view networkLoadLevel) {
    if (transport == "radio") {
        return "radio-baseline";
    }
    if (networkLoadLevel == "idle" || networkLoadLevel == "baseline" || networkLoadLevel.empty()) {
        return "private-5g-baseline";
    }
    return "private-5g-stressed";
}

inline bool consume_experiment_context_arg(ExperimentContext& context,
                                           std::string_view arg,
                                           int& index,
                                           int argc,
                                           char** argv) {
    auto require_value = [&](const char* flag) -> std::string {
        if (index + 1 >= argc) {
            throw std::invalid_argument(std::string("missing value for ") + flag);
        }
        return argv[++index];
    };

    if (arg == "--run-id") {
        context.runId = require_value("--run-id");
        return true;
    }
    if (arg == "--condition-id") {
        context.conditionId = require_value("--condition-id");
        return true;
    }
    if (arg == "--condition-label") {
        context.conditionLabel = require_value("--condition-label");
        return true;
    }
    if (arg == "--request-id") {
        context.requestIdBase = require_value("--request-id");
        return true;
    }
    if (arg == "--av-id") {
        context.avId = require_value("--av-id");
        return true;
    }
    if (arg == "--obu-id") {
        context.obuId = require_value("--obu-id");
        return true;
    }
    if (arg == "--rsu-id") {
        context.rsuId = require_value("--rsu-id");
        return true;
    }
    if (arg == "--network-load-level") {
        context.networkLoadLevel = require_value("--network-load-level");
        return true;
    }
    if (arg == "--qos-profile") {
        context.qosProfile = require_value("--qos-profile");
        return true;
    }
    if (arg == "--mobility-state") {
        context.mobilityState = require_value("--mobility-state");
        return true;
    }
    if (arg == "--clock-sync-state") {
        context.clockSyncState = require_value("--clock-sync-state");
        return true;
    }
    if (arg == "--service-success") {
        context.serviceSuccess = parse_experiment_bool(require_value("--service-success"));
        return true;
    }
    if (arg == "--vehicle-outcome-name") {
        context.vehicleOutcomeName = require_value("--vehicle-outcome-name");
        return true;
    }
    if (arg == "--vehicle-outcome-value") {
        context.vehicleOutcomeValue = require_value("--vehicle-outcome-value");
        return true;
    }
    if (arg == "--vehicle-outcome-unit") {
        context.vehicleOutcomeUnit = require_value("--vehicle-outcome-unit");
        return true;
    }
    return false;
}

} // namespace ipi::api
