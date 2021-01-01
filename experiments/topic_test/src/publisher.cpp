#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

class TestPublisher : public rclcpp::Node
{
public:
    TestPublisher(): Node("test_publisher")
    {
        pub_ = create_publisher<std_msgs::msg::String>("topic", 1);
        timer_ = create_wall_timer(
            500ms, std::bind(&TestPublisher::timer_callback, this)
        );
    }
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int counter_ = 0;
    void timer_callback()
    {
        auto msg = std_msgs::msg::String();
        msg.data = std::to_string(++counter_);
        msg.data = std::string(10 - msg.data.size(), ' ') + msg.data;   // align messages
        // RCLCPP_INFO(get_logger(), "Put: %s", msg.data.c_str());
        std::cout << counter_ << " " <<
            duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << "\n";
        pub_->publish(msg);
    }
};

void run_publisher(int duration)
{
    std::promise<void> p;
    rclcpp::spin_until_future_complete(
        std::make_shared<TestPublisher>(),
        std::shared_future<void>(p.get_future()),
        seconds(duration)
    );
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    std::thread publisherRunner(run_publisher, 10);
    publisherRunner.join();
    rclcpp::shutdown();
    return 0;
}