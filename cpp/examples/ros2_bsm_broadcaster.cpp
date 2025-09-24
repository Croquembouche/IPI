#include <rclcpp/rclcpp.hpp>
#include <v2x_msg/msg/bsm.hpp>

#define uint64_t mocar_uint64_t
#define int64_t mocar_int64_t
#define uint32_t mocar_uint32_t
#define int32_t mocar_int32_t
#define uint16_t mocar_uint16_t
#define int16_t mocar_int16_t
extern "C" {
#include <v2x_api.h>
}
#undef uint64_t
#undef int64_t
#undef uint32_t
#undef int32_t
#undef uint16_t
#undef int16_t

#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"
#include "ipi/v2x/ros2_bridge.hpp"

#include <ctime>
#include <cstring>
#include <stdexcept>

namespace {

constexpr const char* kDefaultLogCfg = "../third_party/mocar/samples/mocar_log.conf";
constexpr const char* kModuleName = "ipi_ros_bsm_broadcaster";
constexpr int kMsgCountModulus = 128;

void convert_to_mocar(const ipi::j2735::BasicSafetyMessage& src, v2x_msg_bsm_t& dst)
{
    static unsigned int msg_counter = 0;

    std::memset(&dst, 0, sizeof(dst));
    dst.msgcount = msg_counter++ % kMsgCountModulus;
    dst.temp_id = src.vehicleId;
    dst.unix_time = std::time(nullptr);

    dst.latitude = src.latitude;
    dst.longitude = src.longitude;
    dst.elevation = 0.0; // optional, could be derived from extensions
    dst.speed = static_cast<double>(src.speedMps) * 3.6; // mocar expects km/h
    dst.heading = static_cast<double>(src.headingDeg);

    if (src.accelerationMps2) {
        dst.longaccel = static_cast<double>(*src.accelerationMps2);
    }

    dst.positionalaccuracy[0] = 1.0;
    dst.positionalaccuracy[1] = 1.0;
    dst.positionalaccuracy[2] = 0.0;
    dst.transmissionstate = 2; // forward gears by default
    dst.vehicletype = 4;
    dst.width = 2.0;
    dst.length = 4.5;
}

class RosBsmBroadcasterNode final : public rclcpp::Node {
public:
    RosBsmBroadcasterNode()
    : Node("ipi_bsm_broadcaster")
    {
        const char* logCfg = std::getenv("MOCAR_LOG_CFG");
        if (!logCfg) {
            logCfg = kDefaultLogCfg;
        }
        if (mde_v2x_init(logCfg, kModuleName) != 0) {
            throw std::runtime_error("Failed to initialise Mocar V2X SDK");
        }

        subscription_ = this->create_subscription<v2x_msg::msg::BSM>(
            "ipi/bsm_tx",
            rclcpp::QoS{10},
            std::bind(&RosBsmBroadcasterNode::on_bsm, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "IPI ROS2 BSM broadcaster initialised");
    }

private:
    void on_bsm(const v2x_msg::msg::BSM::SharedPtr msg)
    {
        try {
            auto model = ipi::v2x::Ros2Bridge::from_ros(*msg);
            codec_.encode(model); // validate through UPER encode

            v2x_msg_bsm_t mocar_msg;
            convert_to_mocar(model, mocar_msg);

            if (mde_v2x_send_bsm(&mocar_msg) != 0) {
                RCLCPP_ERROR(this->get_logger(), "Failed to broadcast BSM for vehicleId=0x%08x",
                             static_cast<uint32_t>(model.vehicleId));
            } else {
                RCLCPP_INFO(this->get_logger(), "Broadcasted BSM tempId=0x%08x",
                            static_cast<uint32_t>(model.vehicleId));
            }
        } catch (const std::exception& ex) {
            RCLCPP_ERROR(this->get_logger(), "Exception while handling BSM: %s", ex.what());
        }
    }

    rclcpp::Subscription<v2x_msg::msg::BSM>::SharedPtr subscription_{};
    ipi::v2x::UperCodec codec_{};
};

} // namespace

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    try {
        auto node = std::make_shared<RosBsmBroadcasterNode>();
        rclcpp::spin(node);
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "Fatal: %s\n", ex.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
