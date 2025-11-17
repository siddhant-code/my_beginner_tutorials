# my_beginner_tutorials

A minimal **ROS 2 Humble** package demonstrating a full **publisher →
subscriber → service** pipeline in C++. This tutorial includes:

-   A **publisher** that periodically publishes string messages.
-   A **subscriber** that receives messages and invokes a **service** to
    modify the text.
-   A **service** that toggles case (upper ↔ lower).
-   Launch file support for **rosbag recording**, TF frame inspection,
    and example tests.

------------------------------------------------------------------------

## Overview

### **Publisher Node**

-   **Executable:** `talker`
-   **Topic:** `/chatter`
-   **Message:** `std_msgs/msg/String`
-   **Function:** Publishes incrementing string messages at
    user-specified frequency (`frequency` parameter).

### **Subscriber Node**

-   **Executable:** `listener`
-   **Topic:** `/chatter`
-   **Function:** Logs incoming messages and calls `/change_case`
    service.

### **Service Node**

-   **Executable:** `server`
-   **Service:** `/change_case`
-   **Type:** `custom_interface/srv/ChangeCase`
-   **Function:** Converts lowercase → uppercase and uppercase →
    lowercase.

------------------------------------------------------------------------

## Dependencies & Assumptions

-  **ROS 2 Distribution:**   **Humble Hawksbill** (Ubuntu 22.04)
-  **Build Tool:**           `colcon`
-  **Language Standard:**    C++14 or later
-  **Dependencies:**         `rclcpp`, `std_msgs`, `custom_interface`
-  **TF Tools:**             `tf2_ros`, `tf2_tools` (optional for inspection)

> **Note:** The `custom_interface` package must provide `ChangeCase.srv`
> before building.

------------------------------------------------------------------------

## Build Instructions

``` bash
# 1. Create workspace
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src

# 2. Clone this package
git clone https://github.com/siddhant-code/my_beginner_tutorials.git

# 3. Build the workspace
cd ~/ros2_ws
colcon build --symlink-install

# 4. Source the workspace
source install/setup.bash
```

------------------------------------------------------------------------

# Running the Package

## Running the Service Node

``` bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials server
```

### Make a service call from another terminal:

``` bash
source ~/ros2_ws/install/setup.bash
ros2 service call /change_case custom_interface/srv/ChangeCase "{input: 'hello world'}"
```

------------------------------------------------------------------------

## Running the Talker and Listener Manually

**Terminal 1:** start the publisher

Provide the required frequency(ms) when running the launch command.

``` bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials talker --ros-args -p frequency:=1000
```

**Terminal 2:** start the subscriber

``` bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials listener
```

------------------------------------------------------------------------

# Launch File Usage

## Run everything using the provided launch file

Provide the required frequency(ms) when running the launch command.

``` bash
ros2 launch beginner_tutorials tutorial_launch.yaml frequency:=2000
```

------------------------------------------------------------------------

# Bag Recording With Launch File

The package includes a launch file that conditionally records ROS bag
files.

### **Enable recording (default):**

Set the record flag as true and provide the path to save the output.

``` bash
ros2 launch beginner_tutorials ros2_bag_recorder.launch.py record:=true output_dir:=my_bag
```

### **Disable recording:**

Set the record flag as false if no recording is to be done. This will just spin up the talker node.

``` bash
ros2 launch beginner_tutorials ros2_bag_recorder.launch.py record:=false
```

------------------------------------------------------------------------

## Inspecting the Bag File

### 1. List information recorded in the bag

``` bash
ros2 bag info my_bag
```

### 2. Playback the bag file for the listener demonstration

Terminal 1 --- run the listener:

``` bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials listener
```

Terminal 2 --- play back the bag:

``` bash
source ~/ros2_ws/install/setup.bash
ros2 bag play my_bag
```

------------------------------------------------------------------------

# Inspecting TF Frames

Ensure the talker node is running before running below commands.

### Generate TF tree PDF:

``` bash
ros2 run tf2_tools view_frames
```

### Echo transforms:

``` bash
ros2 run tf2_ros tf2_echo world talk
```

------------------------------------------------------------------------

# Running ROS 2 Tests

Befire running the tests, ensure package is build and sourced.

``` bash
cd ~/ros2_ws
colcon test  --return-code-on-test-failure --event-handlers console_cohesion+ --packages-select beginner_tutorials
```