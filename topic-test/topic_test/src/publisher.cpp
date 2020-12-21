#include <chrono>
#include <string>
#include <memory>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class TestPublisher : public rclcpp::Node
{
public:
    TestPublisher(): Node("test_publisher")
    {
        pub_ = create_publisher<std_msgs::msg::String>("topic", 50);
        timer_ = create_wall_timer(
            500ms, std::bind(&TestPublisher::timer_callback, this)
        );
    }
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    void timer_callback()
    {
        auto msg = std_msgs::msg::String();
        msg.data = std::string("[this is a string of length 32]#");
        RCLCPP_INFO(get_logger(), "Put: %s", msg.data.c_str());
        pub_->publish(msg);
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TestPublisher>());
    rclcpp::shutdown();
    return 0;
}