//
// Created by xiguang on 2022/6/15.
//

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int32.hpp"
#include "village_interfaces/srv/sell_novel.hpp"
#include <unistd.h>
#include <queue>

class PoorMan: public rclcpp::Node{
public:
    PoorMan(std::string name):Node(name){
        RCLCPP_INFO(get_logger(),"this is %s", name.c_str());
        client_ = create_client<village_interfaces::srv::SellNovel>("sell_novel",rmw_qos_profile_default);
    }

    void ButNovel(){
        RCLCPP_INFO(get_logger(),"buy 10 chapters");
        // 等待服务上线
        while (!client_->wait_for_service(std::chrono::seconds(1))){
            if(!rclcpp::ok()){
                RCLCPP_INFO(get_logger(),"waiting server be interrupted");
                return;
            }
            RCLCPP_INFO(get_logger(), "Waiting for server...");
        }

        auto request = std::make_shared<village_interfaces::srv::SellNovel_Request>();
        request->money = 10;
        client_->async_send_request(request,std::bind(&PoorMan::CallbackNovel,this,
                                                      std::placeholders::_1));
    }

private:
    rclcpp::Client<village_interfaces::srv::SellNovel>::SharedPtr client_;

    void CallbackNovel(rclcpp::Client<village_interfaces::srv::SellNovel>::SharedFuture response){
        RCLCPP_INFO(get_logger(),"start to read");
        for(auto &chapter:response.get()->novels)
            RCLCPP_INFO(get_logger(),"%s",chapter.c_str());
        RCLCPP_INFO(get_logger(),"read over!!");
    }
};

int main(int argc, char **argv){    // 参数个数，参数字符串组
    rclcpp::init(argc,argv);
    // 新建节点
    auto node = std::make_shared<PoorMan>("wang3");
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    node->ButNovel();
    executor.spin(); // 此处阻塞线程，直到ctrl+C,退出线程。
    rclcpp::shutdown();
    return 0;
}