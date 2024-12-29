#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/msg/pose.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

void move_robot_to_pose(
    const std::shared_ptr<rclcpp::Node> node, 
    float x, float y, float z, float roll, float pitch, float yaw)
{
    auto manipulator_move_group = moveit::planning_interface::MoveGroupInterface(node, "manipulator");
    manipulator_move_group.setPlannerId("RRTConnect");  // default
    manipulator_move_group.setPlanningTime(30.0);

    // geometry_msgs::msg::Pose target_pose;
    // target_pose.position.x = x;
    // target_pose.position.y = y;
    // target_pose.position.z = z;
    // tf2::Quaternion quaternion;
    // quaternion.setRPY(roll, pitch, yaw);
    // target_pose.orientation.x = quaternion.x();
    // target_pose.orientation.y = quaternion.y();
    // target_pose.orientation.z = quaternion.z();
    // target_pose.orientation.w = quaternion.w();

    tf2::Quaternion quaternion;
    quaternion.setRPY(roll, pitch, yaw);
    geometry_msgs::msg::Quaternion q_msg = tf2::toMsg(quaternion);
    geometry_msgs::msg::Pose target_pose;
    target_pose.orientation = q_msg;
    target_pose.position.x = x;
    target_pose.position.y = y;
    target_pose.position.z = z;

    bool manipulator_at_goal = manipulator_move_group.setPoseTarget(target_pose);

    if (!manipulator_at_goal)
    {
        RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "INVALID GOAL!");
        return;
    }

    moveit::planning_interface::MoveGroupInterface::Plan manipulator_plan;
    moveit::core::MoveItErrorCode plan_result = manipulator_move_group.plan(manipulator_plan);

    if (plan_result == moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "MANIPULATOR PLAN SUCCEEDED!");
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), moveit::core::error_code_to_string(plan_result).c_str());
        manipulator_move_group.move();
        // manipulator_move_group.execute(manipulator_plan);  // both move() and execute() work
    }
    else
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "MANIPULATOR PLAN FAILED!");
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), moveit::core::error_code_to_string(plan_result).c_str());
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    if (argc != 7)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Usage: <x> <y> <z> <roll> <pitch> <yaw>");
        return 1;
    }

    float x = std::stof(argv[1]);
    float y = std::stof(argv[2]);
    float z = std::stof(argv[3]);
    float roll = std::stof(argv[4]);
    float pitch = std::stof(argv[5]);
    float yaw = std::stof(argv[6]);

    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("interface_pose");
    move_robot_to_pose(node, x, y, z, roll, pitch, yaw);

    rclcpp::shutdown();
    return 0;
}