#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

class Stats
{
public:
    static std::atomic_int message_count;
};

std::atomic_int Stats::message_count;

class TestSubscriber : public rclcpp::Node
{
public:
    TestSubscriber(const char *node_name = "test_subscriber"): Node(node_name)
    {
        sub_ = create_subscription<std_msgs::msg::String>(
            "topic", 1,
            std::bind(&TestSubscriber::topic_callback, this, std::placeholders::_1)
        );
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
    {
        // RCLCPP_INFO(get_logger(), "Get: %s", msg->data.c_str());
        // int n = std::stoi(msg->data);
        // std::cout << n << " " <<
        //     duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << "\n";
        Stats::message_count++;
    }
};

void run_subscriber(const char *node_name, int duration)
{
    std::promise<void> p;
    rclcpp::spin_until_future_complete(
        std::make_shared<TestSubscriber>(node_name),
        std::shared_future<void>(p.get_future()),
        seconds(duration)
    );
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    const int DURATION = 60;
    const int MAX_NODE_COUNT = 16;
    std::vector<std::thread> SubscriberRunners(MAX_NODE_COUNT);
    for (int n = 1; n <= MAX_NODE_COUNT; n++)
    {
        Stats::message_count = 0;
        for (int i = 0; i < n; i++)
            SubscriberRunners[i] = std::thread(
                run_subscriber,
                (std::string("sub_") + std::to_string(i)).c_str(),
                DURATION
            );
        for (int i = 0; i < n; i++)
            SubscriberRunners[i].join();
        std::cout << Stats::message_count << "\n";
        std::cerr << "n=" << n << " completed.\n";
    }
    rclcpp::shutdown();
    return 0;
}