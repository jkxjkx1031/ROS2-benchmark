#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/trigger.hpp"

void respond(const std::shared_ptr<example_interfaces::srv::Trigger::Request> request,
    std::shared_ptr<example_interfaces::srv::Trigger::Response> response)
{
    response->success = true;
    // RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Request received.");
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("server_node");
    rclcpp::Service<example_interfaces::srv::Trigger>::SharedPtr server =
        node->create_service<example_interfaces::srv::Trigger>("test_service", &respond);
    rclcpp::spin(node);
    rclcpp::shutdown();
}