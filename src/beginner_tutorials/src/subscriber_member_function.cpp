/**
 * @file minimal_subscriber.cpp
 * @brief Subscriber node that listens to a topic and calls ChangeCase service.
 */

#include <chrono>
#include <functional>
#include <memory>

#include "custom_interface/srv/change_case.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

/**
 * @class MinimalSubscriber
 * @brief A ROS2 node subscribing to a topic and calling ChangeCase service.
 *
 * Receives string messages on topic "topic", sends them to the "change_case"
 * service, and logs the response.
 */
class MinimalSubscriber : public rclcpp::Node {
 public:
  /**
   * @brief Construct a new MinimalSubscriber object.
   *
   * Initializes the subscriber.
   */
  MinimalSubscriber() : Node("minimal_subscriber") {
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Initializing MinimalSubscriber node...");

    subscription_ = this->create_subscription<std_msgs::msg::String>(
        "topic", 10,
        std::bind(&MinimalSubscriber::TopicCallback, this,
                  std::placeholders::_1));

    RCLCPP_INFO_STREAM(this->get_logger(), "Subscribed to topic 'topic'.");
  }

 private:
  /**
   * @brief Callback function for topic subscription.
   *
   * Sends the received string to the ChangeCase service and logs the output.
   *
   * @param msg Incoming message from topic.
   */
  void TopicCallback(const std_msgs::msg::String &msg) const {
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Raw text received: '" << msg.data << "'");

    auto request =
        std::make_shared<custom_interface::srv::ChangeCase::Request>();
    request->input = msg.data;

    while (!client->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR_STREAM(
            rclcpp::get_logger("rclcpp"),
            "Interrupted while waiting for the service. Exiting.");
      }
      RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                         "Service not available, waiting again...");
    }

    auto result_future = client->async_send_request(request);

    // Wait for the result
    if (rclcpp::spin_until_future_complete(node, result_future) ==
        rclcpp::FutureReturnCode::SUCCESS) {
      RCLCPP_INFO_STREAM(this->get_logger(),
                         "Changed case of text after processing by service: '"
                             << result_future.get()->output << "'");
    } else {
      RCLCPP_ERROR_STREAM(this->get_logger(), "Failed to call service");
    }

    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                       "-------------------------------------------------------"
                       "--------------------------");
  }

  /// Subscriber to the topic
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

  /// Separate node for the service client
  std::shared_ptr<rclcpp::Node> node =
      rclcpp::Node::make_shared("change_case_client");

  /// Service client for ChangeCase service
  rclcpp::Client<custom_interface::srv::ChangeCase>::SharedPtr client =
      node->create_client<custom_interface::srv::ChangeCase>("change_case");
};

/**
 * @brief Main function for the MinimalSubscriber node.
 *
 * Initializes ROS, spins the subscriber, and shuts down.
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return int Exit status
 */
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
