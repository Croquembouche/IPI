#include "ipi/activation/activation.hpp"

#include "ipi_msgs/msg/activation_event.hpp"
#include "ipi_msgs/msg/activation_zone.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "std_msgs/msg/bool.hpp"

#include <chrono>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace {

ipi::api::Timestamp to_timestamp(const builtin_interfaces::msg::Time& value) {
  return ipi::api::Timestamp{
      std::chrono::seconds{value.sec} + std::chrono::nanoseconds{value.nanosec}};
}

rclcpp::QoS durable_qos(std::size_t depth = 10) {
  return rclcpp::QoS(rclcpp::KeepLast(depth)).reliable().transient_local();
}

class ActivationManagerNode final : public rclcpp::Node {
public:
  ActivationManagerNode() : Node("ipi_activation_manager") {
    zone_subscription_ = create_subscription<ipi_msgs::msg::ActivationZone>(
        "/ipi/infrastructure/activation_zones", durable_qos(),
        [this](const ipi_msgs::msg::ActivationZone::SharedPtr message) {
          install_zone(*message);
        });
    position_subscription_ = create_subscription<sensor_msgs::msg::NavSatFix>(
        "/ipi/vehicle/fix", rclcpp::SensorDataQoS(),
        [this](const sensor_msgs::msg::NavSatFix::SharedPtr message) {
          update_position(*message);
        });
    infrastructure_subscription_ = create_subscription<std_msgs::msg::Bool>(
        "/ipi/infrastructure/available", durable_qos(1),
        [this](const std_msgs::msg::Bool::SharedPtr message) {
          infrastructure_available_ = message->data;
        });
    event_publisher_ = create_publisher<ipi_msgs::msg::ActivationEvent>(
        "/ipi/vehicle/activation_event", durable_qos());
  }

private:
  void install_zone(const ipi_msgs::msg::ActivationZone& message) {
    try {
      if (message.latitudes.size() != message.longitudes.size()) {
        throw std::invalid_argument("activation zone latitude/longitude sizes differ");
      }
      ipi::activation::ActivationZone zone;
      zone.zoneId = message.zone_id;
      zone.revision = message.revision;
      zone.validFrom = to_timestamp(message.valid_from);
      zone.expiresAt = to_timestamp(message.expires_at);
      zone.enterHysteresisMeters = message.enter_hysteresis_m;
      zone.exitHysteresisMeters = message.exit_hysteresis_m;
      zone.minimumDwell = std::chrono::milliseconds{message.minimum_dwell_ms};
      if (message.geometry_type == ipi_msgs::msg::ActivationZone::CIRCLE) {
        zone.geofence.type = ipi::api::GeometryType::CIRCLE;
        zone.geofence.radiusMeters = message.radius_m;
      } else if (message.geometry_type == ipi_msgs::msg::ActivationZone::POLYGON) {
        zone.geofence.type = ipi::api::GeometryType::POLYGON;
      } else {
        throw std::invalid_argument("unknown activation zone geometry type");
      }
      for (std::size_t index = 0; index < message.latitudes.size(); ++index) {
        zone.geofence.points.push_back(
            ipi::api::GeoPoint{message.latitudes[index], message.longitudes[index], {}});
      }
      for (const auto action : message.actions) {
        if (action > static_cast<std::uint8_t>(
                         ipi::activation::ActivationAction::SERVICE_MODE_HINT)) {
          throw std::invalid_argument("unknown activation action");
        }
        zone.actions.push_back(static_cast<ipi::activation::ActivationAction>(action));
      }
      const bool installed = tracker_.set_zone(std::move(zone),
                                                to_timestamp(message.stamp));
      if (installed) {
        RCLCPP_INFO(get_logger(), "installed activation zone %s revision %lu",
                    message.zone_id.c_str(),
                    static_cast<unsigned long>(message.revision));
      }
    } catch (const std::exception& error) {
      RCLCPP_ERROR(get_logger(), "rejected activation zone: %s", error.what());
    }
  }

  void update_position(const sensor_msgs::msg::NavSatFix& message) {
    if (!std::isfinite(message.latitude) || !std::isfinite(message.longitude)) {
      RCLCPP_WARN(get_logger(), "ignoring non-finite vehicle position");
      return;
    }
    try {
      const auto update = tracker_.update(
          ipi::api::GeoPoint{message.latitude, message.longitude, message.altitude},
          std::chrono::steady_clock::now(), std::chrono::system_clock::now(),
          infrastructure_available_);
      if (!update.stateChanged) {
        return;
      }
      ipi_msgs::msg::ActivationEvent event;
      event.stamp = static_cast<builtin_interfaces::msg::Time>(now());
      event.zone_id = update.zoneId;
      event.revision = update.revision;
      event.previous_state = static_cast<std::uint8_t>(update.previousState);
      event.state = static_cast<std::uint8_t>(update.state);
      event.sequence = update.sequence;
      event.reason = static_cast<std::uint8_t>(update.reason);
      for (const auto action : update.enabledActions) {
        event.enabled_actions.push_back(static_cast<std::uint8_t>(action));
      }
      event_publisher_->publish(event);
    } catch (const std::exception& error) {
      RCLCPP_ERROR(get_logger(), "activation update failed: %s", error.what());
    }
  }

  ipi::activation::ActivationZoneTracker tracker_;
  bool infrastructure_available_{true};
  rclcpp::Subscription<ipi_msgs::msg::ActivationZone>::SharedPtr zone_subscription_;
  rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr position_subscription_;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr infrastructure_subscription_;
  rclcpp::Publisher<ipi_msgs::msg::ActivationEvent>::SharedPtr event_publisher_;
};

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ActivationManagerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
