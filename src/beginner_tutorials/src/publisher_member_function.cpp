/**
 * @file minimal_publisher.cpp
 * @brief A simple ROS2 publisher node that publishes string messages
 * periodically.
 */

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

/**
 * @class MinimalPublisher
 * @brief Publishes messages to a topic at a user-defined frequency.
 *
 * The node publishes a string message on the "topic" topic at the
 * frequency specified by the ROS parameter "frequency".
 */
class MinimalPublisher : public rclcpp::Node {
 public:
  /**
   * @brief Construct a new MinimalPublisher object.
   *
   * Initializes publisher, reads frequency parameter, and starts the timer.
   */
  MinimalPublisher() : Node("minimal_publisher"), count_(0) {
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Initializing MinimalPublisher node...");

    // Create publisher
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Publisher for topic 'topic' created.");

    // Declare and get frequency parameter
    this->declare_parameter<int>("frequency");
    FREQUENCY = this->get_parameter("frequency").as_int();
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Publishing frequency set to " << FREQUENCY << " ms.");

    // Create timer for periodic publishing
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(FREQUENCY),
        std::bind(&MinimalPublisher::TimerCallback, this));
  }

 protected:
  /// Publishing frequency in milliseconds
  int FREQUENCY;

 private:
  /**
   * @brief Timer callback to publish string messages.
   *
   * Each message includes an incrementing counter.
   */
  void TimerCallback() {
    std_msgs::msg::String message;
    message.data = "this is a custom message! " + std::to_string(count_++);

    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Publishing: '" << message.data << "'");
    publisher_->publish(message);
  }

  /// Timer for periodic publishing
  rclcpp::TimerBase::SharedPtr timer_;

  /// Publisher object for sending messages
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;

  /// Message counter
  size_t count_;
};

/**
 * @brief Main function to run the MinimalPublisher node.
 *
 * Initializes ROS, spins the node, and shuts down gracefully.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit code (0 on success)
 */
int main(int argc, char **argv) {
  RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                     "Starting MinimalPublisher node...");
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
