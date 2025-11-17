/**
 * @file integration_test_node.cpp
 * @author Siddhant
 * @brief Integration tests for ROS 2 publisher, subscriber, and static TF.
 * @version 0.2
 * @date 2025-11-17
 *
 * This test file validates:
 *  - Publishing on the "chatter" topic.
 *  - Reception of topic data by a subscriber within a timeout.
 *  - Presence and correctness of static transform published on /tf_static.
 */

#include <catch_ros2/catch_ros2.hpp>
#include <chrono>
#include <cmath>
#include <rclcpp/executors.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <tf2_msgs/msg/tf_message.hpp>

using namespace std::chrono_literals;
using std_msgs::msg::String;

/// Global logger used inside test callbacks
auto Logger = rclcpp::get_logger("");

/**
 * @class MyTestsFixture
 * @brief Test fixture used for initializing ROS nodes and parameters.
 *
 * Sets up:
 *  - A dedicated test node (`IntegrationTestNode1`).
 *  - A configurable test duration parameter.
 */
class MyTestsFixture {
 public:
  /**
   * @brief Constructor initializes the test fixture.
   *
   * Creates the test node and retrieves the test duration parameter.
   */
  MyTestsFixture() {
    // Create test node
    tester_node_ = rclcpp::Node::make_shared("IntegrationTestNode1");
    Logger = tester_node_->get_logger();

    RCLCPP_INFO_STREAM(Logger, "IntegrationTestNode1 initialized.");

    // Declare and retrieve the test duration parameter
    tester_node_->declare_parameter<double>("test_duration");
    test_duration_ = tester_node_->get_parameter("test_duration")
                         .get_parameter_value()
                         .get<double>();

    RCLCPP_INFO_STREAM(Logger, "Loaded test_duration = " << test_duration_);
  }

  /**
   * @brief Destructor (empty, provided for completeness).
   */
  ~MyTestsFixture() = default;

 protected:
  double test_duration_;  ///< Duration allowed for receiving topic data
  rclcpp::Node::SharedPtr
      tester_node_;  ///< Node used for performing test operations
};

/**
 * @test Verify that "chatter" topic is received within the timeout.
 *
 * This test subscribes to the "chatter" topic and waits for a message
 * to arrive within the specified duration.
 */
TEST_CASE_METHOD(MyTestsFixture, "test topic talker", "[topic]") {
  bool got_topic = false;

  /**
   * @brief Listener callback to process received messages.
   */
  struct ListenerCallback {
    explicit ListenerCallback(bool &topic_flag) : topic_flag_(topic_flag) {}

    void operator()(const String msg) const {
      RCLCPP_INFO_STREAM(Logger, "Received message: " << msg.data);
      topic_flag_ = true;
    }

    bool &topic_flag_;
  };

  // Create subscriber
  auto subscriber = tester_node_->create_subscription<String>(
      "chatter", 10, ListenerCallback(got_topic));

  RCLCPP_INFO_STREAM(Logger,
                     "Subscribed to 'chatter', waiting for messages...");

  // Timing
  rclcpp::Rate rate(10.0);  // Check at 10 Hz
  auto start_time = rclcpp::Clock().now();
  auto duration = rclcpp::Clock().now() - start_time;
  auto timeout = rclcpp::Duration::from_seconds(test_duration_);

  RCLCPP_DEBUG_STREAM(Logger, "Starting topic wait loop. Timeout = "
                                  << timeout.seconds() << " seconds.");

  // Spin until received or timed out
  while (!got_topic && duration < timeout) {
    rclcpp::spin_some(tester_node_);
    rate.sleep();
    duration = rclcpp::Clock().now() - start_time;
  }

  RCLCPP_INFO_STREAM(
      Logger,
      "Final duration = " << duration.seconds() << ", got_topic=" << got_topic);

  CHECK(got_topic);  // Assert message was received
}

/**
 * @test Verify the static transform published on /tf_static.
 *
 * The test:
 *   - Subscribes to /tf_static using transient local QoS
 *   - Waits for a transform message
 *   - Compares received transform to expected values
 */
TEST_CASE("verify static transform on /tf_static", "[tf]") {
  // Test node
  auto node = std::make_shared<rclcpp::Node>("tf_static_test_node");

  RCLCPP_INFO_STREAM(node->get_logger(),
                     "Starting test for static transform on /tf_static.");

  // QoS for latched static transform
  rclcpp::QoS qos(1);
  qos.transient_local();
  qos.reliable();

  bool received = false;
  geometry_msgs::msg::Transform tf_out;

  // Callback to capture the static transform
  auto callback = [&](const tf2_msgs::msg::TFMessage &msg) {
    if (!msg.transforms.empty()) {
      tf_out = msg.transforms[0].transform;
      received = true;

      RCLCPP_INFO_STREAM(node->get_logger(),
                         "Received static transform from frame: "
                             << msg.transforms[0].header.frame_id);
    }
  };

  auto sub = node->create_subscription<tf2_msgs::msg::TFMessage>("tf_static",
                                                                 qos, callback);

  RCLCPP_INFO_STREAM(node->get_logger(),
                     "Subscribed to '/tf_static', waiting for transform...");

  // Wait for transform
  rclcpp::Rate rate(20);
  auto start = node->get_clock()->now();
  auto timeout = rclcpp::Duration::from_seconds(6.0);

  while (!received && (node->get_clock()->now() - start) < timeout) {
    rclcpp::spin_some(node);
    rate.sleep();
  }

  CHECK(received);  // Must receive a transform

  // Expected translation
  const double ex = 5.0;
  const double ey = 3.0;
  const double ez = 1.0;

  // Expected quaternion
  const double qx = 0.034270798550482096;
  const double qy = 0.10602051106179562;
  const double qz = 0.1435721750273919;
  const double qw = 0.9833474432563558;

  const double eps = 1e-6;

  // Translation tests
  CHECK(std::abs(tf_out.translation.x - ex) < eps);
  CHECK(std::abs(tf_out.translation.y - ey) < eps);
  CHECK(std::abs(tf_out.translation.z - ez) < eps);

  // Rotation tests
  CHECK(std::abs(tf_out.rotation.x - qx) < eps);
  CHECK(std::abs(tf_out.rotation.y - qy) < eps);
  CHECK(std::abs(tf_out.rotation.z - qz) < eps);
  CHECK(std::abs(tf_out.rotation.w - qw) < eps);

  RCLCPP_INFO_STREAM(node->get_logger(),
                     "Static transform validated successfully.");
}
