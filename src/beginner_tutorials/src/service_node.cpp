/**
 * @file service_node.cpp
 * @brief Implements a ROS2 service node that toggles the case of characters in
 * a given string.
 */

#include "custom_interface/srv/change_case.hpp"
#include "rclcpp/rclcpp.hpp"

using ChangeCase = custom_interface::srv::ChangeCase;

/**
 * @class ServiceNode
 * @brief A ROS2 node that provides a service to change the case of input
 * strings.
 *
 * This node listens for service requests on the "change_case" service.
 * It converts lowercase letters to uppercase and vice versa, returning the
 * modified string.
 */
class ServiceNode : public rclcpp::Node {
 public:
  /**
   * @brief Construct a new ServiceNode object.
   *
   * Initializes the node and creates a ROS2 service named "change_case".
   */
  ServiceNode() : Node("service_node") {
    RCLCPP_INFO_STREAM(this->get_logger(), "Initializing ServiceNode...");

    try {
      service_ = this->create_service<ChangeCase>(
          "change_case",
          std::bind(&ServiceNode::HandleService, this, std::placeholders::_1,
                    std::placeholders::_2));

      RCLCPP_INFO_STREAM(this->get_logger(),
                         "Service 'change_case' created successfully.");
    } catch (const std::exception &e) {
      RCLCPP_FATAL_STREAM(this->get_logger(),
                          "Failed to create service: " << e.what());
      throw;
    }
  }

 private:
  /**
   * @brief Callback function that handles the ChangeCase service requests.
   *
   * @param request The request message containing the input string.
   * @param response The response message containing the case-changed output
   * string.
   */
  void HandleService(const std::shared_ptr<ChangeCase::Request> request,
                     std::shared_ptr<ChangeCase::Response> response) {
    RCLCPP_DEBUG_STREAM(this->get_logger(),
                        "Received request: \"" << request->input << "\"");

    if (request->input.empty()) {
      RCLCPP_WARN_STREAM(this->get_logger(),
                         "Received empty string. Returning empty response.");
      response->output = "";
      return;
    }

    std::string result;
    for (const char c : request->input) {
      if (std::islower(static_cast<unsigned char>(c))) {
        result += std::toupper(static_cast<unsigned char>(c));
      } else if (std::isupper(static_cast<unsigned char>(c))) {
        result += std::tolower(static_cast<unsigned char>(c));
      } else {
        result += c;
      }
    }

    response->output = result;

    RCLCPP_INFO_STREAM(this->get_logger(),
                       "Processed string successfully. Sending response: \""
                           << response->output << "\"");

    // Additional debug logging for diagnostics
    RCLCPP_DEBUG_STREAM(this->get_logger(),
                        "Original length: " << request->input.size()
                                            << ", Processed length: "
                                            << response->output.size());
  }

  /// ROS2 service handle for the ChangeCase service.
  rclcpp::Service<ChangeCase>::SharedPtr service_;
};

/**
 * @brief Main function for the ServiceNode.
 *
 * Initializes the ROS2 system, spins the node, and shuts down gracefully.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit code (0 on success).
 */
int main(int argc, char **argv) {
  RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                     "Starting ChangeCase Service Node...");

  try {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ServiceNode>());
  } catch (const std::exception &e) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("rclcpp"),
                        "Exception caught in main: " << e.what());
    return 1;
  } catch (...) {
    RCLCPP_FATAL_STREAM(rclcpp::get_logger("rclcpp"),
                        "Unknown fatal error occurred.");
    return 2;
  }

  RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                     "Shutting down ChangeCase Service Node...");
  rclcpp::shutdown();
  return 0;
}
