from pathlib import Path

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    config = str(
        Path(get_package_share_directory("ipi_runtime"))
        / "config"
        / "reference_intersection.yaml"
    )
    return LaunchDescription(
        [
            Node(
                package="ipi_runtime",
                executable="activation_zone_publisher_node",
                name="ipi_activation_zone_publisher",
                parameters=[config],
            ),
            Node(
                package="ipi_runtime",
                executable="activation_manager_node",
                name="ipi_activation_manager",
            ),
            Node(
                package="ipi_runtime",
                executable="tiered_service_manager_node",
                name="ipi_tiered_service_manager",
            ),
            Node(
                package="ipi_runtime",
                executable="offload_decision_node",
                name="ipi_offload_decision",
            ),
        ]
    )
