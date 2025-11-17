/**
 * @file publisher_member_function.cpp
 * @author Siddhant
 * @brief A minimal ROS 2 publisher node that publishes messages and broadcasts
 * a static transform.
 * @version 0.2
 * @date 2025-11-17
 *
 * This file defines the MinimalPublisher class, which publishes string messages
 * at a user-defined frequency and broadcasts a static TF transform.
 */

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "tf2/LinearMath/Quaternion.hpp"
#include "tf2_ros/static_transform_broadcaster.h"

/**
 * @class MinimalPublisher
 * @brief A simple ROS 2 node that publishes messages and broadcasts a static
 * transform.
 *
 * This node publishes string messages on the `/chatter` topic using a frequency
 * defined by the `frequency` ROS parameter. It also broadcasts a predefined
 * static transform between frames `"world"` and `"talk"` at startup.
 */
class MinimalPublisher : public rclcpp::Node {
 public:
  /**
   * @brief Construct a new MinimalPublisher node.
   *
   * Initializes the publisher, reads the frequency parameter, creates a timer,
   * and broadcasts static transforms.
   */
  MinimalPublisher() : Node("minimal_publisher"), count_(0) {
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Initializing MinimalPublisher node...");

    // Create publisher for /chatter topic
    publisher_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Publisher created on topic '/chatter'.");

    // Declare the frequency parameter
    this->declare_parameter<int>("frequency", 1000);
    FREQUENCY = this->get_parameter("frequency").as_int();

    if (FREQUENCY <= 0) {
      RCLCPP_WARN_STREAM(
          this->get_logger(),
          "Frequency parameter is non-positive. Setting frequency to default: "
              << 1000 << " ms.");
      FREQUENCY = 1000;
    }

    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Publishing frequency set to " << FREQUENCY << " ms.");

    // Create timer callback
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(FREQUENCY),
        std::bind(&MinimalPublisher::TimerCallback, this));

    // Initialize TF static broadcaster
    tf_static_broadcaster_ =
        std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

    // Broadcast static transforms once at startup
    RCLCPP_INFO_STREAM(this->get_logger(), "Broadcasting static transform...");
    this->MakeTransforms();
  }

 protected:
  /// Publishing frequency in milliseconds.
  int FREQUENCY;

 private:
  /**
   * @brief Timer callback that publishes string messages.
   *
   * Appends an incrementing counter value to the message.
   */
  void TimerCallback() {
    std_msgs::msg::String message;
    message.data = "this is a custom message! " + std::to_string(count_++);

    RCLCPP_DEBUG_STREAM(this->get_logger(), "Timer callback triggered.");

    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Publishing: '" << message.data << "'");

    publisher_->publish(message);
  }

  /**
   * @brief Broadcasts a static transform between "world" and "talk" frames.
   *
   * This transform is published once at node startup.
   */
  void MakeTransforms() {
    geometry_msgs::msg::TransformStamped transform;

    transform.header.stamp = this->get_clock()->now();
    transform.header.frame_id = "world";
    transform.child_frame_id = "talk";

    // Set translation
    transform.transform.translation.x = 5.0;
    transform.transform.translation.y = 3.0;
    transform.transform.translation.z = 1.0;

    // Set rotation via roll-pitch-yaw
    tf2::Quaternion q;
    q.setRPY(0.1, 0.2, 0.3);

    transform.transform.rotation.x = q.x();
    transform.transform.rotation.y = q.y();
    transform.transform.rotation.z = q.z();
    transform.transform.rotation.w = q.w();

    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Sending static transform: world -> talk");

    tf_static_broadcaster_->sendTransform(transform);
  }

  /// Timer used for periodic message publishing.
  rclcpp::TimerBase::SharedPtr timer_;

  /// Publisher for string messages on /chatter.
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;

  /// Incrementing message counter.
  size_t count_;

  /// Static transform broadcaster for publishing TF frames.
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_static_broadcaster_;
};

/**
 * @brief Entry point for the MinimalPublisher node.
 *
 * Initializes ROS 2, starts the node, and handles shutdown.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Status code (0 for success).
 */
int main(int argc, char **argv) {
  RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                     "Starting MinimalPublisher node...");

  try {
    rclcpp::init(argc, argv);
  } catch (const std::exception &e) {
    RCLCPP_FATAL_STREAM(rclcpp::get_logger("rclcpp"),
                        "Failed to initialize ROS: " << e.what());
    return 1;
  }

  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
