# my_beginner_tutorials

A minimal ROS 2 Humble package demonstrating a complete publisher–subscriber–service communication pattern in C++.
This tutorial showcases:

- A publisher node that periodically publishes string messages.
- A subscriber node that receives those messages and calls a service to modify the text.
- A service node that processes incoming strings by changing their case (upper ↔ lower).
---

## Overview

### Publisher Node
- **Node name:** `minimal_publisher`  
- **Topic:** `/topic`  
- **Message type:** `std_msgs/msg/String`  
- **Functionality:** Publishes incrementing string messages at a configurable frequency (set via parameter frequency).

### Subscriber Node
- **Node name:** `minimal_subscriber`  
- **Topic:** `/topic`  
- **Message type:** `std_msgs/msg/String`  
- **Functionality:** Receives published messages, logs them, and sends the text to the change_case service for processing.

## Service Node
 **Node name:**  service_node
 **Service name:** /change_case
 **Service type:** custom_interface/srv/ChangeCase
 **Functionality:** Converts all lowercase characters to uppercase and vice versa.

---

##  Dependencies and Assumptions

| Component | Version / Notes |
|------------|----------------|
| **ROS 2 Distribution** | **Humble Hawksbill** (tested on Ubuntu 22.04) |
| **C++ Standard** | C++17 or later |
| **Build Tool** | `colcon` |
| **Build System** | `ament_cmake` |
| **Dependencies** | `rclcpp`, `std_msgs`,`custom_interface` |

Note: The custom_interface package must define the ChangeCase service before building this package.

---

##  Build Instructions

Make sure you have **ROS 2 Humble** installed and sourced before building.

```bash
# 1. Create a new workspace
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src

# 2. Clone this repository
git clone https://github.com/siddhant-code/my_beginner_tutorials.git

# 3. Build the package
cd ~/ros2_ws
colcon build

# 4. Source the setup file
source install/setup.bash
```

# Running the Nodes

## Service Call

To make a service call:

In new terminal, run the follwoing commands to start the service:
```bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials server
```

In another terminal, run the following commands to to make a service call

```bash
source ~/ros2_ws/install/setup.bash
ros2 service call /change_case custom_interface/srv/ChangeCase "{input: 'hello world'}"
```
## Talker and listener node

Open two terminals, source the workspace in each, and run the nodes: 

In Terminal 1, for talker

```bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials talker --ros-args -p frequency:=1000
```

In Terminal 2, for listener

```bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials listener
```

## Launch

Alternatively, all the nodes and service can be convieniently launched using one single command:

```bash
ros2  launch beginner_tutorials tutorial_launch.yaml frequency:=2000
```