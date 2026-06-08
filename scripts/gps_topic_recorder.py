#!/usr/bin/env python3
"""Record NovAtel ROS 2 GPS position and speed samples to CSV."""

import argparse
import csv
import math
import time
from dataclasses import dataclass
from pathlib import Path

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from nav_msgs.msg import Odometry
from sensor_msgs.msg import NavSatFix
from novatel_oem7_msgs.msg import BESTPOS, BESTVEL, INSPVA


@dataclass
class Position:
    latitude_deg: float = math.nan
    longitude_deg: float = math.nan
    altitude_m: float = math.nan
    source: str = ""


@dataclass
class Speed:
    horizontal_mps: float = math.nan
    vertical_mps: float = math.nan
    north_mps: float = math.nan
    east_mps: float = math.nan
    up_mps: float = math.nan
    track_ground_deg: float = math.nan
    source: str = ""


class GpsTopicRecorder(Node):
    def __init__(self, output_path: Path, snapshot_path: Path):
        super().__init__("ipi_gps_topic_recorder")
        self.output_path = output_path
        self.snapshot_path = snapshot_path
        self.position = Position()
        self.speed = Speed()
        self.csv_file = output_path.open("w", newline="")
        self.writer = csv.DictWriter(
            self.csv_file,
            fieldnames=[
                "system_time_ns",
                "ros_time_sec",
                "sample_topic",
                "position_source",
                "speed_source",
                "latitude_deg",
                "longitude_deg",
                "altitude_m",
                "horizontal_speed_mps",
                "vertical_speed_mps",
                "north_velocity_mps",
                "east_velocity_mps",
                "up_velocity_mps",
                "track_ground_deg",
                "solution_status",
                "position_or_velocity_type",
            ],
        )
        self.writer.writeheader()
        self.csv_file.flush()

        self.create_subscription(BESTPOS, "/novatel/oem7/bestpos", self.on_bestpos, 10)
        self.create_subscription(BESTVEL, "/novatel/oem7/bestvel", self.on_bestvel, 10)
        self.create_subscription(INSPVA, "/novatel/oem7/inspva", self.on_inspva, 10)
        self.create_subscription(NavSatFix, "/novatel/oem7/fix", self.on_fix, 10)
        self.create_subscription(Odometry, "/novatel/oem7/odom", self.on_odom, 10)

    def destroy_node(self):
        try:
            self.csv_file.flush()
            self.csv_file.close()
        finally:
            super().destroy_node()

    def ros_time_sec(self, msg):
        stamp = getattr(getattr(msg, "header", None), "stamp", None)
        if stamp is None:
            return ""
        return f"{stamp.sec}.{stamp.nanosec:09d}"

    def enum_text(self, value):
        return str(value)

    def write_sample(self, topic, msg, solution_status="", position_or_velocity_type=""):
        row = {
            "system_time_ns": time.time_ns(),
            "ros_time_sec": self.ros_time_sec(msg),
            "sample_topic": topic,
            "position_source": self.position.source,
            "speed_source": self.speed.source,
            "latitude_deg": self.format_float(self.position.latitude_deg),
            "longitude_deg": self.format_float(self.position.longitude_deg),
            "altitude_m": self.format_float(self.position.altitude_m),
            "horizontal_speed_mps": self.format_float(self.speed.horizontal_mps),
            "vertical_speed_mps": self.format_float(self.speed.vertical_mps),
            "north_velocity_mps": self.format_float(self.speed.north_mps),
            "east_velocity_mps": self.format_float(self.speed.east_mps),
            "up_velocity_mps": self.format_float(self.speed.up_mps),
            "track_ground_deg": self.format_float(self.speed.track_ground_deg),
            "solution_status": solution_status,
            "position_or_velocity_type": position_or_velocity_type,
        }
        self.writer.writerow(row)
        self.csv_file.flush()
        self.write_snapshot(row)

    def write_snapshot(self, row):
        tmp_path = self.snapshot_path.with_suffix(self.snapshot_path.suffix + ".tmp")
        with tmp_path.open("w", newline="") as f:
            writer = csv.DictWriter(f, fieldnames=self.writer.fieldnames)
            writer.writeheader()
            writer.writerow(row)
        tmp_path.replace(self.snapshot_path)

    @staticmethod
    def format_float(value):
        if value is None or math.isnan(value):
            return ""
        return f"{value:.9f}"

    def on_bestpos(self, msg):
        self.position = Position(
            latitude_deg=msg.lat,
            longitude_deg=msg.lon,
            altitude_m=msg.hgt,
            source="/novatel/oem7/bestpos",
        )
        self.write_sample(
            "/novatel/oem7/bestpos",
            msg,
            self.enum_text(msg.sol_status),
            self.enum_text(msg.pos_type),
        )

    def on_bestvel(self, msg):
        self.speed = Speed(
            horizontal_mps=msg.hor_speed,
            vertical_mps=msg.ver_speed,
            track_ground_deg=msg.trk_gnd,
            source="/novatel/oem7/bestvel",
        )
        self.write_sample(
            "/novatel/oem7/bestvel",
            msg,
            self.enum_text(msg.sol_status),
            self.enum_text(msg.vel_type),
        )

    def on_inspva(self, msg):
        horizontal = math.hypot(msg.north_velocity, msg.east_velocity)
        self.position = Position(
            latitude_deg=msg.latitude,
            longitude_deg=msg.longitude,
            altitude_m=msg.height,
            source="/novatel/oem7/inspva",
        )
        self.speed = Speed(
            horizontal_mps=horizontal,
            vertical_mps=msg.up_velocity,
            north_mps=msg.north_velocity,
            east_mps=msg.east_velocity,
            up_mps=msg.up_velocity,
            source="/novatel/oem7/inspva",
        )
        self.write_sample("/novatel/oem7/inspva", msg, self.enum_text(msg.status), "")

    def on_fix(self, msg):
        self.position = Position(
            latitude_deg=msg.latitude,
            longitude_deg=msg.longitude,
            altitude_m=msg.altitude,
            source="/novatel/oem7/fix",
        )
        self.write_sample("/novatel/oem7/fix", msg, str(msg.status.status), "")

    def on_odom(self, msg):
        linear = msg.twist.twist.linear
        horizontal = math.hypot(linear.x, linear.y)
        self.speed = Speed(
            horizontal_mps=horizontal,
            vertical_mps=linear.z,
            north_mps=linear.x,
            east_mps=linear.y,
            up_mps=linear.z,
            source="/novatel/oem7/odom",
        )
        self.write_sample("/novatel/oem7/odom", msg, "", "")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, help="CSV path for GPS samples")
    parser.add_argument("--snapshot", required=True, help="CSV path containing the latest sample")
    args = parser.parse_args()

    output_path = Path(args.output)
    snapshot_path = Path(args.snapshot)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    snapshot_path.parent.mkdir(parents=True, exist_ok=True)

    rclpy.init()
    node = GpsTopicRecorder(output_path, snapshot_path)
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    except Exception as exc:
        if "context is not valid" not in str(exc):
            raise
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()
