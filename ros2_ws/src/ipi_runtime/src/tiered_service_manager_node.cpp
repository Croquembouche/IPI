#include "ipi/policy/service_policy.hpp"

#include "ipi_msgs/msg/activation_event.hpp"
#include "ipi_msgs/msg/link_status.hpp"
#include "ipi_msgs/msg/policy_decision.hpp"
#include "ipi_msgs/msg/service_intent.hpp"
#include "rclcpp/rclcpp.hpp"

#include <chrono>
#include <memory>

namespace {

rclcpp::QoS durable_qos(std::size_t depth = 10) {
  return rclcpp::QoS(rclcpp::KeepLast(depth)).reliable().transient_local();
}

class TieredServiceManagerNode final : public rclcpp::Node {
public:
  TieredServiceManagerNode() : Node("ipi_tiered_service_manager") {
    decision_publisher_ = create_publisher<ipi_msgs::msg::PolicyDecision>(
        "/ipi/vehicle/policy_decision", durable_qos());
    activation_subscription_ = create_subscription<ipi_msgs::msg::ActivationEvent>(
        "/ipi/vehicle/activation_event", durable_qos(),
        [this](const ipi_msgs::msg::ActivationEvent::SharedPtr message) {
          if (message->state <= ipi_msgs::msg::ActivationEvent::FALLBACK) {
            activation_state_ =
                static_cast<ipi::activation::ActivationState>(message->state);
          }
        });
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
    intent_subscription_ = create_subscription<ipi_msgs::msg::ServiceIntent>(
        "/ipi/vehicle/service_intent", durable_qos(),
        [this](const ipi_msgs::msg::ServiceIntent::SharedPtr message) {
          evaluate(*message);
        });
  }

private:
  void evaluate(const ipi_msgs::msg::ServiceIntent& message) {
    ipi::policy::ServiceIntent intent;
    intent.requestId = message.request_id;
    intent.sessionId = message.session_id;
    intent.serviceType = static_cast<ipi::ServiceType>(message.service_type);
    intent.tier = static_cast<ipi::policy::ServiceTier>(message.tier);
    intent.deadline = std::chrono::milliseconds{message.deadline_ms};
    intent.maximumRtt = std::chrono::milliseconds{message.maximum_rtt_ms};
    intent.maximumLossFraction = message.maximum_loss_fraction;
    intent.minimumRateHz = message.minimum_rate_hz;
    intent.localFallbackAvailable = message.local_fallback_available;
    intent.currentlyActive = message.currently_active;

    const auto decision = policy_.evaluate(intent, link_, activation_state_);
    ipi_msgs::msg::PolicyDecision output;
    output.stamp = static_cast<builtin_interfaces::msg::Time>(now());
    output.request_id = decision.requestId;
    output.action = static_cast<std::uint8_t>(decision.action);
    output.selected_transport = decision.selectedTransport
                                    ? static_cast<std::uint8_t>(*decision.selectedTransport)
                                    : ipi_msgs::msg::PolicyDecision::NO_TRANSPORT;
    output.maximum_rate_hz = decision.maximumRateHz;
    output.validity_ms = decision.validity.count();
    output.reason = static_cast<std::uint8_t>(decision.reason);
    decision_publisher_->publish(output);
  }

  ipi::policy::TieredServicePolicy policy_;
  ipi::activation::ActivationState activation_state_{
      ipi::activation::ActivationState::OUTSIDE};
  ipi::policy::LinkSnapshot link_{};
  rclcpp::Subscription<ipi_msgs::msg::ActivationEvent>::SharedPtr activation_subscription_;
  rclcpp::Subscription<ipi_msgs::msg::LinkStatus>::SharedPtr link_subscription_;
  rclcpp::Subscription<ipi_msgs::msg::ServiceIntent>::SharedPtr intent_subscription_;
  rclcpp::Publisher<ipi_msgs::msg::PolicyDecision>::SharedPtr decision_publisher_;
};

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TieredServiceManagerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
