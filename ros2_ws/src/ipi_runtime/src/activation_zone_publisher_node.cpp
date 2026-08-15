#include "ipi_msgs/msg/activation_zone.hpp"
#include "rclcpp/rclcpp.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

namespace {

class ActivationZonePublisherNode final : public rclcpp::Node {
public:
  ActivationZonePublisherNode() : Node("ipi_activation_zone_publisher") {
    declare_parameter<std::string>("zone_id", "reference-intersection");
    declare_parameter<std::int64_t>("revision", 1);
    declare_parameter<std::vector<double>>("latitudes", {42.300000});
    declare_parameter<std::vector<double>>("longitudes", {-83.700000});
    declare_parameter<double>("radius_m", 80.0);
    declare_parameter<double>("enter_hysteresis_m", 5.0);
    declare_parameter<double>("exit_hysteresis_m", 5.0);
    declare_parameter<std::int64_t>("minimum_dwell_ms", 250);
    declare_parameter<std::int64_t>("validity_seconds", 86400);
    declare_parameter<std::vector<std::int64_t>>(
        "actions", {ipi_msgs::msg::ActivationZone::MODEL_PULL,
                    ipi_msgs::msg::ActivationZone::TELEMETRY_PUSH,
                    ipi_msgs::msg::ActivationZone::ENABLE_EDGE_PERCEPTION});

    publisher_ = create_publisher<ipi_msgs::msg::ActivationZone>(
        "/ipi/infrastructure/activation_zones",
        rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local());
    timer_ = create_wall_timer(std::chrono::milliseconds(100), [this] { publish_once(); });
  }

private:
  void publish_once() {
    ipi_msgs::msg::ActivationZone zone;
    const auto current = now();
    zone.stamp = static_cast<builtin_interfaces::msg::Time>(current);
    zone.zone_id = get_parameter("zone_id").as_string();
    zone.revision = static_cast<std::uint64_t>(get_parameter("revision").as_int());
    zone.geometry_type = ipi_msgs::msg::ActivationZone::CIRCLE;
    zone.latitudes = get_parameter("latitudes").as_double_array();
    zone.longitudes = get_parameter("longitudes").as_double_array();
    zone.radius_m = get_parameter("radius_m").as_double();
    zone.valid_from = static_cast<builtin_interfaces::msg::Time>(
        current - rclcpp::Duration::from_seconds(1.0));
    zone.expires_at = static_cast<builtin_interfaces::msg::Time>(
        current + rclcpp::Duration::from_seconds(
                      static_cast<double>(get_parameter("validity_seconds").as_int())));
    zone.enter_hysteresis_m = get_parameter("enter_hysteresis_m").as_double();
    zone.exit_hysteresis_m = get_parameter("exit_hysteresis_m").as_double();
    zone.minimum_dwell_ms =
        static_cast<std::uint32_t>(get_parameter("minimum_dwell_ms").as_int());
    for (const auto action : get_parameter("actions").as_integer_array()) {
      if (action >= 0 &&
          action <= ipi_msgs::msg::ActivationZone::SERVICE_MODE_HINT) {
        zone.actions.push_back(static_cast<std::uint8_t>(action));
      }
    }
    publisher_->publish(zone);
    timer_->cancel();
    RCLCPP_INFO(get_logger(), "published activation zone %s revision %lu",
                zone.zone_id.c_str(), static_cast<unsigned long>(zone.revision));
  }

  rclcpp::Publisher<ipi_msgs::msg::ActivationZone>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ActivationZonePublisherNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
