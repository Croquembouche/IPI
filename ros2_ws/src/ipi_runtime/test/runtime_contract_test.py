import time
import unittest

import launch
import launch_ros.actions
import launch_testing
import launch_testing.actions
import launch_testing.asserts
import rclpy
from ipi_msgs.msg import (
    ActivationEvent,
    LinkStatus,
    OffloadDecision,
    OffloadRequest,
    PolicyDecision,
    ServiceIntent,
)
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from sensor_msgs.msg import NavSatFix


def generate_test_description():
    nodes = [
        launch_ros.actions.Node(
            package="ipi_runtime",
            executable="activation_zone_publisher_node",
            parameters=[
                {
                    "zone_id": "launch-test-zone",
                    "revision": 1,
                    "latitudes": [42.3],
                    "longitudes": [-83.7],
                    "radius_m": 80.0,
                    "enter_hysteresis_m": 1.0,
                    "exit_hysteresis_m": 1.0,
                    "minimum_dwell_ms": 50,
                    "validity_seconds": 60,
                    "actions": [0, 1, 2],
                }
            ],
        ),
        launch_ros.actions.Node(
            package="ipi_runtime", executable="activation_manager_node"
        ),
        launch_ros.actions.Node(
            package="ipi_runtime", executable="tiered_service_manager_node"
        ),
        launch_ros.actions.Node(
            package="ipi_runtime", executable="offload_decision_node"
        ),
    ]
    return launch.LaunchDescription(nodes + [launch_testing.actions.ReadyToTest()])


class RuntimeContractTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        rclpy.init()
        cls.node = rclpy.create_node("ipi_runtime_contract_test")
        cls.durable_qos = QoSProfile(
            depth=10,
            reliability=ReliabilityPolicy.RELIABLE,
            durability=DurabilityPolicy.TRANSIENT_LOCAL,
        )
        cls.activation = None
        cls.policy = None
        cls.offload = None
        cls.node.create_subscription(
            ActivationEvent,
            "/ipi/vehicle/activation_event",
            lambda message: setattr(cls, "activation", message),
            cls.durable_qos,
        )
        cls.node.create_subscription(
            PolicyDecision,
            "/ipi/vehicle/policy_decision",
            lambda message: setattr(cls, "policy", message),
            cls.durable_qos,
        )
        cls.node.create_subscription(
            OffloadDecision,
            "/ipi/vehicle/offload_decision",
            lambda message: setattr(cls, "offload", message),
            cls.durable_qos,
        )
        cls.fix_publisher = cls.node.create_publisher(
            NavSatFix, "/ipi/vehicle/fix", 10
        )
        cls.link_publisher = cls.node.create_publisher(
            LinkStatus, "/ipi/vehicle/link_status", cls.durable_qos
        )
        cls.intent_publisher = cls.node.create_publisher(
            ServiceIntent, "/ipi/vehicle/service_intent", cls.durable_qos
        )
        cls.offload_publisher = cls.node.create_publisher(
            OffloadRequest, "/ipi/vehicle/offload_request", cls.durable_qos
        )

    @classmethod
    def tearDownClass(cls):
        cls.node.destroy_node()
        rclpy.shutdown()

    @classmethod
    def spin_until(cls, predicate, timeout=5.0):
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            rclpy.spin_once(cls.node, timeout_sec=0.1)
            if predicate():
                return True
        return False

    @classmethod
    def publish_link(cls, available, congestion=0.0):
        message = LinkStatus()
        message.transport = LinkStatus.CELLULAR_5G
        message.available = available
        message.service_available = True
        message.rtt_ms = 20
        message.packet_loss_fraction = 0.0
        message.congestion_fraction = congestion
        message.estimated_uplink_mbps = 100.0
        message.estimated_downlink_mbps = 100.0
        cls.link_publisher.publish(message)
        rclpy.spin_once(cls.node, timeout_sec=0.15)

    def test_activation_policy_and_offload_contract(self):
        self.assertTrue(
            self.spin_until(
                lambda: self.fix_publisher.get_subscription_count() > 0
                and self.link_publisher.get_subscription_count() >= 2
            )
        )

        fix = NavSatFix()
        fix.latitude = 42.3
        fix.longitude = -83.7
        fix.altitude = 200.0
        # The durable zone advertisement can arrive just after the graph is
        # connected. Re-publish the same pose through one full dwell window;
        # the state machine emits at most one event for each transition.
        for _ in range(4):
            self.fix_publisher.publish(fix)
            rclpy.spin_once(self.node, timeout_sec=0.15)
        self.assertTrue(
            self.spin_until(
                lambda: self.activation is not None
                and self.activation.state == ActivationEvent.ACTIVE
            )
        )

        self.publish_link(True, congestion=0.9)
        best_effort = ServiceIntent()
        best_effort.request_id = "best-effort-test"
        best_effort.session_id = "session-test"
        best_effort.service_type = ServiceIntent.HD_MAP_UPDATE
        best_effort.tier = ServiceIntent.BEST_EFFORT
        best_effort.deadline_ms = 500
        best_effort.maximum_rtt_ms = 100
        best_effort.maximum_loss_fraction = 0.05
        best_effort.minimum_rate_hz = 5.0
        best_effort.local_fallback_available = True
        best_effort.currently_active = True
        self.intent_publisher.publish(best_effort)
        self.assertTrue(
            self.spin_until(
                lambda: self.policy is not None
                and self.policy.request_id == best_effort.request_id
            )
        )
        self.assertEqual(self.policy.action, PolicyDecision.PAUSE)
        self.assertEqual(self.policy.reason, PolicyDecision.CONGESTED)

        self.publish_link(False)
        safety = ServiceIntent()
        safety.request_id = "safety-test"
        safety.session_id = "session-test"
        safety.service_type = ServiceIntent.PERCEPTION_AID
        safety.tier = ServiceIntent.SAFETY_CRITICAL
        safety.deadline_ms = 100
        safety.maximum_rtt_ms = 50
        safety.maximum_loss_fraction = 0.01
        safety.minimum_rate_hz = 10.0
        safety.local_fallback_available = True
        safety.currently_active = True
        self.intent_publisher.publish(safety)
        self.assertTrue(
            self.spin_until(
                lambda: self.policy is not None
                and self.policy.request_id == safety.request_id
            )
        )
        self.assertEqual(self.policy.action, PolicyDecision.USE_LOCAL_FALLBACK)

        request = OffloadRequest()
        request.request_id = "offload-local-test"
        request.deadline_ms = 300
        request.local_queue_ms = 50
        request.local_compute_ms = 250
        request.upload_bytes = 100000
        request.download_bytes = 10000
        request.edge_queue_ms = 10
        request.edge_compute_ms = 30
        request.uncertainty_margin_ms = 10
        request.minimum_edge_benefit_ms = 5
        request.minimum_result_confidence = 0.8
        request.expected_result_confidence = 0.95
        request.edge_service_available = True
        request.local_fallback_available = True
        self.offload_publisher.publish(request)
        self.assertTrue(
            self.spin_until(
                lambda: self.offload is not None
                and self.offload.request_id == request.request_id
            )
        )
        self.assertEqual(self.offload.execution_site, OffloadDecision.LOCAL)
        self.assertTrue(self.offload.fallback_required)

        self.publish_link(True, congestion=0.1)
        request.request_id = "offload-edge-test"
        self.offload_publisher.publish(request)
        self.assertTrue(
            self.spin_until(
                lambda: self.offload is not None
                and self.offload.request_id == request.request_id
            )
        )
        self.assertEqual(self.offload.execution_site, OffloadDecision.EDGE)
        self.assertFalse(self.offload.fallback_required)


@launch_testing.post_shutdown_test()
class RuntimeShutdownTest(unittest.TestCase):
    def test_all_runtime_processes_exit_cleanly(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)
