#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy

class OdomRelayNode(Node):
    def __init__(self):
        super().__init__('odom_relay_node')

        # 设置日志级别为 DEBUG，方便调试
        self.get_logger().set_level(rclpy.logging.LoggingSeverity.INFO)

        # 设置与原始 /odom 话题发布者一致的 QoS（BEST_EFFORT）
        qos = QoSProfile(
            depth=10,
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST
        )

        # 创建订阅者：监听 /odom
        self.odom_sub = self.create_subscription(
            Odometry,
            '/odom',
            self.odom_callback,
            qos
        )
        self.get_logger().info('已订阅话题 /odom')

        # 创建发布者：转发到 /odom_UI
        self.odom_pub = self.create_publisher(
            Odometry,
            '/odom_UI',
            10
        )
        self.get_logger().info('发布器准备完成，发布至 /odom_UI')

    def odom_callback(self, msg: Odometry):
        # self.get_logger().info(
        #     f'📥 收到 /odom 消息: vx={msg.twist.twist.linear.x:.3f} m/s, '
        #     f'wz={msg.twist.twist.angular.z:.3f} rad/s'
        # )
        self.odom_pub.publish(msg)
        # self.get_logger().info('已转发至 /odom_UI')

def main(args=None):
    rclpy.init(args=args)
    node = OdomRelayNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('节点中断退出')
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
