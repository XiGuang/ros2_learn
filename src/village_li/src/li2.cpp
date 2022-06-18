//
// Created by xiguang on 2022/6/14.
//

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int32.hpp"
#include "village_interfaces/srv/sell_novel.hpp"
#include <unistd.h>

class Writer: public rclcpp::Node{
public:
    explicit Writer(const std::string& name):Node(name){
        RCLCPP_INFO(get_logger(),"this is %s",name.c_str());
        subscription_money = create_subscription<std_msgs::msg::UInt32>(
                "Money",10,
                std::bind(&Writer::topicCallback,this,std::placeholders::_1));
        publisher_novel = create_publisher<std_msgs::msg::String>("Beautiful_New_World",10);
        std_msgs::msg::String str;
        str.data = "0 chapter";
        publisher_novel->publish(str);
    }

private:
    rclcpp::Subscription<std_msgs::msg::UInt32>::SharedPtr subscription_money;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_novel;

    void topicCallback(const std_msgs::msg::UInt32::SharedPtr money){
        static int num = 1;
        RCLCPP_INFO(get_logger(), "%s get %d $, new chapter %d is coming", get_name(), money->data, num);
        sleep(1);
        std_msgs::msg::String str;
        str.data = "new capture" + std::to_string(num++);
        publisher_novel->publish(str);
    }

};

int main(int argc, char **argv){
    rclcpp::init(argc,argv);
    auto node = std::make_shared<Writer>("Li2");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}