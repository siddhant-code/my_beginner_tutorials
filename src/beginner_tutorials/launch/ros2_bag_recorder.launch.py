from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition

def generate_launch_description():

    # Declare a launch argument named "record"
    # This flag controls whether rosbag recording should be enabled.
    record_arg = DeclareLaunchArgument(
        'record',
        default_value='true',
        description='Enable or disable bag recording'
    )

    # Access the value of the "record" argument at runtime.
    # LaunchConfiguration defers evaluation until launch time.
    record_flag = LaunchConfiguration('record')

    # Declare an argument for specifying the output directory where bag files will be stored.
    # Only required if recording is enabled (condition = IfCondition(record_flag)).
    output_dir = DeclareLaunchArgument(
        'output_dir',
        condition = IfCondition(record_flag),
        description='Directory to store the recorded bag files'
    )

    # Define a ROS 2 Node that publishes messages.
    # Here, the talker node runs with parameter 'frequency' set to 500.
    talker_node = Node(
        package="beginner_tutorials",
        executable="talker",
        name="talker",
        output="screen",
        parameters=[{'frequency': 500}]
    )

    # Execute rosbag2 recording as an external process.
    # Runs only if "record" == true.
    # Records all topics (-a) into the directory specified by output_dir.
    record_bag = ExecuteProcess(
        condition=IfCondition(record_flag),
        cmd=['ros2', 'bag', 'record', '-a', '-o', LaunchConfiguration('output_dir')],
        output='screen'
    )

    # Add all actions to the launch description in the order they should be processed.
    return LaunchDescription([
        record_arg,
        output_dir,
        talker_node,
        record_bag
    ])
