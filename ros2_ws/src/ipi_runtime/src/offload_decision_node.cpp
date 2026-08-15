#include "ipi/offload/offload_decision.hpp"

#include "ipi_msgs/msg/link_status.hpp"
#include "ipi_msgs/msg/offload_decision.hpp"
#include "ipi_msgs/msg/offload_request.hpp"
#include "rclcpp/rclcpp.hpp"

#include <chrono>
#include <limits>
#include <memory>

namespace {

rclcpp::QoS durable_qos(std::size_t depth = 10) {
  return rclcpp::QoS(rclcpp::KeepLast(depth)).reliable().transient_local();
}

class OffloadDecisionNode final : public rclcpp::Node {
public:
  OffloadDecisionNode() : Node("ipi_offload_decision") {
    decision_publisher_ = create_publisher<ipi_msgs::msg::OffloadDecision>(
        "/ipi/vehicle/offload_decision", durable_qos());
    link_subscription_ = create_subscription<ipi_msgs::msg::LinkStatus>(
        "/ipi/vehicle/link_status", durable_qos(),
        [this](const ipi_msgs::msg::LinkStatus::SharedPtr message) {
          link_.transport = static_cast<ipi::api::TransportType>(message->transport);
          link_.available = message->available;
          link_.serviceAvailable = message->service_available;
          link_.rtt = std::chrono::milliseconds{message->rtt_ms};
          link_.packetLossFraction = message->packet_loss_fraction;
          link_.congestionFraction = message->congestion_fraction;
          link_.estimatedUplinkMbps = message->estimated_uplink_mbps;
          link_.estimatedDownlinkMbps = message->estimated_downlink_mbps;
        });
    request_subscription_ = create_subscription<ipi_msgs::msg::OffloadRequest>(
        "/ipi/vehicle/offload_request", durable_qos(),
        [this](const ipi_msgs::msg::OffloadRequest::SharedPtr message) {
          evaluate(*message);
        });
  }

private:
  void evaluate(const ipi_msgs::msg::OffloadRequest& message) {
    ipi::offload::OffloadInputs inputs;
    inputs.requestId = message.request_id;
    inputs.deadline = std::chrono::milliseconds{message.deadline_ms};
    inputs.localQueueTime = std::chrono::milliseconds{message.local_queue_ms};
    inputs.localComputeTime = std::chrono::milliseconds{message.local_compute_ms};
    if (message.upload_bytes > std::numeric_limits<std::size_t>::max() ||
        message.download_bytes > std::numeric_limits<std::size_t>::max()) {
      inputs.requestId.clear();
    } else {
      inputs.uploadBytes = static_cast<std::size_t>(message.upload_bytes);
      inputs.downloadBytes = static_cast<std::size_t>(message.download_bytes);
    }
    inputs.link = link_;
    inputs.edgeQueueTime = std::chrono::milliseconds{message.edge_queue_ms};
    inputs.edgeComputeTime = std::chrono::milliseconds{message.edge_compute_ms};
    inputs.uncertaintyMargin = std::chrono::milliseconds{message.uncertainty_margin_ms};
    inputs.minimumEdgeBenefit = std::chrono::milliseconds{message.minimum_edge_benefit_ms};
    inputs.minimumResultConfidence = message.minimum_result_confidence;
    inputs.expectedResultConfidence = message.expected_result_confidence;
    inputs.edgeServiceAvailable = message.edge_service_available;
    inputs.localFallbackAvailable = message.local_fallback_available;

    const auto decision = engine_.evaluate(inputs);
    ipi_msgs::msg::OffloadDecision output;
    output.stamp = static_cast<builtin_interfaces::msg::Time>(now());
    output.request_id = message.request_id;
    output.execution_site = static_cast<std::uint8_t>(decision.executionSite);
    output.predicted_local_completion_ms = decision.predictedLocalCompletion.count();
    output.predicted_edge_completion_ms = decision.predictedEdgeCompletion.count();
    output.deadline_expected_to_be_met = decision.deadlineExpectedToBeMet;
    output.fallback_required = decision.fallbackRequired;
    output.reason = static_cast<std::uint8_t>(decision.reason);
    decision_publisher_->publish(output);
  }

  ipi::offload::OffloadDecisionEngine engine_;
  ipi::policy::LinkSnapshot link_{};
  rclcpp::Subscription<ipi_msgs::msg::LinkStatus>::SharedPtr link_subscription_;
  rclcpp::Subscription<ipi_msgs::msg::OffloadRequest>::SharedPtr request_subscription_;
  rclcpp::Publisher<ipi_msgs::msg::OffloadDecision>::SharedPtr decision_publisher_;
};

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<OffloadDecisionNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
