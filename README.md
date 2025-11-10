# my_beginner_tutorials

A minimal **ROS 2 Humble** example demonstrating a simple **publisher–subscriber** communication pattern in C++.  
It includes two nodes:

- **`MinimalPublisher`** — publishes a string message periodically.  
- **`MinimalSubscriber`** — subscribes to the same topic and logs the received messages.

---

## Overview

### Publisher Node
- **Node name:** `minimal_publisher`  
- **Topic:** `/topic`  
- **Message type:** `std_msgs/msg/String`  
- **Functionality:** Publishes incrementing string messages.

### Subscriber Node
- **Node name:** `minimal_subscriber`  
- **Topic:** `/topic`  
- **Message type:** `std_msgs/msg/String`  
- **Functionality:** Logs received messages using ROS 2 logging utilities.

---

##  Dependencies and Assumptions

| Component | Version / Notes |
|------------|----------------|
| **ROS 2 Distribution** | **Humble Hawksbill** (tested on Ubuntu 22.04) |
| **C++ Standard** | C++17 or later |
| **Build Tool** | `colcon` |
| **Build System** | `ament_cmake` |
| **Dependencies** | `rclcpp`, `std_msgs` |

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
colcon build --packages-select beginner_tutorials

# 4. Source the setup file
source install/setup.bash
```

## Running the Nodes

Open two terminals, source the workspace in each, and run the nodes: 

In Terminal 1, for talker

```bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials talker
```


In Terminal 2, for listener

```bash
source ~/ros2_ws/install/setup.bash
ros2 run beginner_tutorials listener
```