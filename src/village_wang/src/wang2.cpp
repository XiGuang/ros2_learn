//
// Created by xiguang on 2022/6/14.
//

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int32.hpp"
#include "village_interfaces/srv/sell_novel.hpp"
#include <unistd.h>
#include <queue>

class People:public rclcpp::Node{
public:
    explicit People(const std::string &name):Node(name){
        RCLCPP_INFO(get_logger(),"hello world! I`m %s", name.c_str());
        // 订阅
        subscription_novel_ = create_subscription<std_msgs::msg::String>(
                "Beautiful_New_World",10,
                std::bind(&People::topicCallback, this, std::placeholders::_1));
        // 发布
        publisher_money_ = create_publisher<std_msgs::msg::UInt32>("Money", 10);
        // 获得回调函数线程组
        callback_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        // 服务
        server_ = create_service<village_interfaces::srv::SellNovel>("sell_novel",
                                                                     std::bind(&People::serveCallback,this,
                                                                               std::placeholders::_1,
                                                                               std::placeholders::_2),
                                                                     rmw_qos_profile_default,
                                                                     callback_group_);
    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_novel_;
    rclcpp::Publisher<std_msgs::msg::UInt32>::SharedPtr publisher_money_;

    rclcpp::CallbackGroup::SharedPtr callback_group_;

    rclcpp::Service<village_interfaces::srv::SellNovel>::SharedPtr server_;

    std::queue<std::string> chapters_;

    void serveCallback(const village_interfaces::srv::SellNovel::Request::SharedPtr request,
                       const village_interfaces::srv::SellNovel::Response::SharedPtr response){
        RCLCPP_INFO(get_logger(),"got %d $, will give %d chapters", request->money, request->money);

        rclcpp::Rate loop_rate(1);
        while(request->money > chapters_.size()){
            if(!rclcpp::ok()) {   // 系统寄了
                RCLCPP_INFO(get_logger(),"system error");
                return;
            }

            RCLCPP_INFO(get_logger(),"Had %d chapter, waiting for another %d chapter(s)",
                        chapters_.size(), request->money - chapters_.size());
            loop_rate.sleep();
        }

        RCLCPP_INFO(get_logger(),"Had enough %d chapters", request->money);
        for(unsigned int i = 0; i < request->money; ++i){
            response->novels.emplace_back(chapters_.front());
            chapters_.pop();
        }
    }

    void topicCallback(const std_msgs::msg::String::SharedPtr novel){
        std_msgs::msg::UInt32 money;
        money.data = 10;
        sleep(1);
        RCLCPP_INFO(get_logger(), "GOT %s and pay %d $", novel->data.c_str(), money.data);
        publisher_money_->publish(money);
        chapters_.push(novel->data);
    }

};

int main(int argc, char **argv){    // 参数个数，参数字符串组
    rclcpp::init(argc,argv);
    // 新建节点
    auto node = std::make_shared<People>("wang2");
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin(); // 此处阻塞线程，直到ctrl+C,退出线程。
    rclcpp::shutdown();
    return 0;
}