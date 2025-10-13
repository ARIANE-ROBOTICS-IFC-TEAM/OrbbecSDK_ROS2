from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, GroupAction, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description() -> LaunchDescription:
    # Include launch files
    package_dir = get_package_share_directory("orbbec_camera")
    launch_file_dir = os.path.join(package_dir, "launch")

    width = "640"
    height = "400"
    fps = "15"

    launch1_include = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(launch_file_dir, "gemini_330_series.launch.py")
        ),
        launch_arguments={
            "enumerate_net_device": "true",
            "camera_name": "camera_01",
            "net_device_ip": "192.168.10.101",
            "net_device_port": "8090",
            "sync_mode": "standalone",
            "color_width": width,
            "color_height": height,
            "color_fps": fps,
            "color_format": "MJPG",
            "depth_width": width,
            "depth_height": height,
            "depth_fps": fps,
            "left_ir_width": width,
            "left_ir_height": height,
            "left_ir_fps": fps,
            "right_ir_width": width,
            "right_ir_height": height,
            "right_ir_fps": fps,
            "enable_sync_output_accel_gyro": "true",
        }.items(),
    )

    launch2_include = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(launch_file_dir, "gemini_330_series.launch.py")
        ),
        launch_arguments={
            "enumerate_net_device": "true",
            "camera_name": "camera_02",
            "net_device_ip": "192.168.10.102",
            "net_device_port": "8090",
            "sync_mode": "standalone",
            "color_width": width,
            "color_height": height,
            "color_fps": fps,
            "color_format": "MJPG",
            "depth_width": width,
            "depth_height": height,
            "depth_fps": fps,
            "left_ir_width": width,
            "left_ir_height": height,
            "left_ir_fps": fps,
            "right_ir_width": width,
            "right_ir_height": height,
            "right_ir_fps": fps,
            "enable_sync_output_accel_gyro": "true",
        }.items(),
    )

    launch3_include = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(launch_file_dir, "gemini_330_series.launch.py")
        ),
        launch_arguments={
            "enumerate_net_device": "true",
            "camera_name": "camera_03",
            "net_device_ip": "192.168.10.103",
            "net_device_port": "8090",
            "sync_mode": "standalone",
            "color_width": width,
            "color_height": height,
            "color_fps": fps,
            "color_format": "MJPG",
            "depth_width": width,
            "depth_height": height,
            "depth_fps": fps,
            "left_ir_width": width,
            "left_ir_height": height,
            "left_ir_fps": fps,
            "right_ir_width": width,
            "right_ir_height": height,
            "right_ir_fps": fps,
            "enable_sync_output_accel_gyro": "true",
        }.items(),
    )

    launch4_include = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(launch_file_dir, "gemini_330_series.launch.py")
        ),
        launch_arguments={
            "enumerate_net_device": "true",
            "camera_name": "camera_04",
            "net_device_ip": "192.168.10.104",
            "net_device_port": "8090",
            "sync_mode": "standalone",
            "color_width": width,
            "color_height": height,
            "color_fps": fps,
            "color_format": "MJPG",
            "depth_width": width,
            "depth_height": height,
            "depth_fps": fps,
            "left_ir_width": width,
            "left_ir_height": height,
            "left_ir_fps": fps,
            "right_ir_width": width,
            "right_ir_height": height,
            "right_ir_fps": fps,
            "enable_sync_output_accel_gyro": "true",
        }.items(),
    )

    # Launch description
    ld = LaunchDescription(
        [
            TimerAction(period=0.0, actions=[GroupAction([launch1_include])]),
            # TimerAction(period=2.0, actions=[GroupAction([launch2_include])]),
            # TimerAction(period=4.0, actions=[GroupAction([launch3_include])]),
            # TimerAction(period=6.0, actions=[GroupAction([launch4_include])]),
        ]
    )

    return ld
