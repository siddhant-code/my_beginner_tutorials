/**
 * @file subscriber_member_function.cpp
 * @author Siddhant
 * @brief ROS 2 subscriber node that calls a ChangeCase service.
 * @version 0.2
 * @date 2025-11-17
 *
 * This file defines the MinimalSubscriber class, which subscribes to the
 * "chatter" topic and calls the "change_case" service to modify text written
 * in the received messages.
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
 * @brief A ROS 2 node subscribing to text messages and invoking a ChangeCase
 * service.
 *
 * The node listens to the "chatter" topic and forwards each received message
 * to the "change_case" service. The service response is logged.
 */
class MinimalSubscriber : public rclcpp::Node {
 public:
  /**
   * @brief Construct a new MinimalSubscriber node.
   *
   * Creates a subscription to the "chatter" topic and sets up a service client
   * for calling the ChangeCase service.
   */
  MinimalSubscriber() : Node("minimal_subscriber") {
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Initializing MinimalSubscriber node...");

    // Create subscription to /chatter
    subscription_ = this->create_subscription<std_msgs::msg::String>(
        "chatter", 10,
        std::bind(&MinimalSubscriber::TopicCallback, this,
                  std::placeholders::_1));

    RCLCPP_INFO_STREAM(this->get_logger(), "Subscribed to topic '/chatter'.");

    // Log that the client node was created
    RCLCPP_DEBUG_STREAM(
        this->get_logger(),
        "Service client node 'change_case_client' created successfully.");
  }

 private:
  /**
   * @brief Topic callback executed whenever a message is received.
   *
   * Sends the text to the ChangeCase service and logs the formatted output.
   *
   * @param msg The received message.
   */
  void TopicCallback(const std_msgs::msg::String &msg) const {
    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Received text: '" << msg.data << "'");

    // Create service request
    auto request =
        std::make_shared<custom_interface::srv::ChangeCase::Request>();
    request->input = msg.data;

    // Wait for the service to become available
    if (!client->wait_for_service(10ms)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR_STREAM(
            this->get_logger(),
            "ROS interrupted while waiting for service 'change_case'.");
        return;
      }

      RCLCPP_WARN_STREAM(this->get_logger(),
                         "Service 'change_case' not available. Make sure the "
                         "server is running.");
      return;
    }

    // Call the service asynchronously
    auto result_future = client->async_send_request(request);

    RCLCPP_DEBUG_STREAM(this->get_logger(),
                        "Request sent to ChangeCase service.");

    // Wait and process result
    auto result_status =
        rclcpp::spin_until_future_complete(node, result_future);

    if (result_status == rclcpp::FutureReturnCode::SUCCESS) {
      RCLCPP_INFO_STREAM(this->get_logger(), "Service response: '"
                                                 << result_future.get()->output
                                                 << "'");
    } else {
      RCLCPP_ERROR_STREAM(this->get_logger(),
                          "Failed to call service 'change_case'.");
    }

    RCLCPP_INFO_STREAM(
        this->get_logger(),
        "-------------------------------------------------------------");
  }

  /// Subscriber to "chatter" topic.
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

  /// Node used for creating and spinning the service client.
  std::shared_ptr<rclcpp::Node> node =
      rclcpp::Node::make_shared("change_case_client");

  /// Client for calling ChangeCase service.
  rclcpp::Client<custom_interface::srv::ChangeCase>::SharedPtr client =
      node->create_client<custom_interface::srv::ChangeCase>("change_case");
};

/**
 * @brief Entry point for the MinimalSubscriber node.
 *
 * Initializes ROS 2, spins the subscriber node, and shuts down.
 *
 * @param argc Command-line argument count.
 * @param argv Command-line arguments.
 * @return int Exit code.
 */
int main(int argc, char **argv) {
  RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                     "Launching MinimalSubscriber node...");

  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();

  RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                     "MinimalSubscriber node shutdown complete.");

  return 0;
}
