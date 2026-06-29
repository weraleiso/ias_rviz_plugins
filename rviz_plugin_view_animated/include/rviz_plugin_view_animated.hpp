#ifndef RVIZ_PLUGIN_VIEW_ANIMATED_HPP
#define RVIZ_PLUGIN_VIEW_ANIMATED_HPP

// ROS2 libraries
#include<rclcpp/rclcpp.hpp>
#include<rcl_interfaces/msg/parameter_event.hpp>
#include<ament_index_cpp/get_package_share_directory.hpp>
#include<image_transport/image_transport.hpp>
#include<cv_bridge/cv_bridge.hpp>
#include<std_msgs/msg/bool.hpp>
#include<std_msgs/msg/float32.hpp>
#include<geometry_msgs/msg/pose.hpp>
#include<geometry_msgs/msg/point_stamped.hpp>
#include<geometry_msgs/msg/vector3_stamped.hpp>
#include<geometry_msgs/msg/transform_stamped.hpp>
#include<sensor_msgs/msg/image.hpp>
#include<sensor_msgs/msg/compressed_image.hpp>
#include<sensor_msgs/image_encodings.hpp>
#include<rviz_plugin_view_animated_msgs/msg/view_movement.hpp>
#include<rviz_plugin_view_animated_msgs/msg/view_trajectory.hpp>
#include<tf2_ros/transform_broadcaster.h>

// OpenCV libraries
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

// RViz libraries
#include<rviz_common/ros_integration/ros_node_abstraction_iface.hpp>
#include<rviz_common/message_filter_display.hpp>
#include<rviz_common/logging.hpp>
#include<rviz_common/frame_manager_iface.hpp>
#include<rviz_common/view_controller.hpp>
#include<rviz_common/view_manager.hpp>
#include<rviz_common/frame_manager_iface.hpp>
#include<rviz_common/render_panel.hpp>
#include<rviz_common/load_resource.hpp>
#include<rviz_common/display_context.hpp>
#include<rviz_common/viewport_mouse_event.hpp>
#include<rviz_rendering/objects/shape.hpp>
#include<rviz_common/properties/bool_property.hpp>
#include<rviz_common/properties/int_property.hpp>
#include<rviz_common/properties/editable_enum_property.hpp>
#include<rviz_common/properties/float_property.hpp>
#include<rviz_common/properties/tf_frame_property.hpp>
#include<rviz_common/properties/ros_topic_property.hpp>
#include<rviz_common/properties/vector_property.hpp>
#include<rviz_common/properties/quaternion_property.hpp>
#include<rviz_common/properties/color_property.hpp>
#include<rviz_rendering/render_window.hpp>
#include<rviz_rendering/objects/shape.hpp>

// OGRE libraries
#include<Ogre.h>
#include<OgreRoot.h>
#include<OgreSceneManager.h>
#include<OgreSceneNode.h>
#include<OgreEntity.h>
#include<OgreFrustum.h>
#include<OgreMaterialManager.h>
#include<OgreMovableObject.h>
#include<OgreManualObject.h>
#include<OgreRenderQueueListener.h>
#include<OgreRenderSystem.h>
#include<OgreRenderTargetListener.h>
#include<OgreRenderWindow.h>
#include<OgreHardwarePixelBuffer.h>
#include<OgrePixelFormat.h>

// STANDARD libraries
#include<iostream>
#include<ctime>
#include<iomanip>
#include<sstream>



namespace rviz_plugin_view_animated
{
    static uint32_t si_resource_name_count; // Make resource names unique

    class RVizPluginViewAnimated:public rviz_common::ViewController
    {
        Q_OBJECT

        // ROS2 properties
        rclcpp::Node::SharedPtr m_hdl_node;
        std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> m_hdl_node_rviz;
        rclcpp::Subscription<rviz_plugin_view_animated_msgs::msg::ViewTrajectory>::SharedPtr m_sub_vtr_view_trajectory;
        rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr m_sub_f32_view_pause;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr m_pub_bol_view_animation_finished;
        rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr m_pub_pst_view;
        rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr m_pub_img_view_live;
        std::unique_ptr<tf2_ros::TransformBroadcaster> tf2_broadcaster;
        geometry_msgs::msg::TransformStamped m_tfs_transform;
        geometry_msgs::msg::PoseStamped m_msg_pst_camera;
        std::vector<rviz_plugin_view_animated_msgs::msg::ViewMovement> m_vec_vmo_movements;
        sensor_msgs::msg::CompressedImage m_msg_img_view_live;
        rclcpp::Time m_tim_view_animation_start;

        // OpenCV properties
        cv::VideoWriter m_vwt_view_live;
        cv::Size m_siz_view_live_frame;
        cv::Mat m_mat_view_live_frame;

        // RViz properties
        std::unique_ptr<rviz_common::properties::TfFrameProperty> m_prp_view_tf_frame;
        std::unique_ptr<rviz_common::properties::EditableEnumProperty> m_prp_view_mode;
        std::unique_ptr<rviz_common::properties::EditableEnumProperty> m_prp_view_speed_profile;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_view_duration;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_view_progress;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_view_target_fps;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_eye_local;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_focus_local;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_up_local;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_eye;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_focus;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_up;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_view_direction;
        std::unique_ptr<rviz_common::properties::QuaternionProperty> m_prp_view_orientation;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_view_distance;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_frame_by_frame_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_vector_up_lock_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_pose_publish_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_mouse_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_animation_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_live_publish_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_view_live_record_enabled;
        std::unique_ptr<rviz_common::properties::StringProperty> m_prp_view_live_record_path;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_view_live_width;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_view_live_height;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_view_live_color;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_view_live_topic;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_view_pose_topic;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_view_animation_completed_topic;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_view_animation_pause_topic;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_view_trajectory_topic;
        std::unique_ptr<rviz_rendering::Shape> m_shp_focal;

        // OGRE properties
        Ogre::TexturePtr m_tep_view_live;
        Ogre::RenderTexture* m_p_ret_view_live;
        Ogre::Vector3 m_ov3_focus_old;
        Ogre::Vector3 m_ov3_focus_local_old;
        Ogre::Vector3 m_ov3_tf_frame_position;
        Ogre::Quaternion m_oqu_tf_frame_orientation;

        // Helper properties
        QCursor m_qcu_mouse_control_disabled;
        std::vector<int> m_vec_view_live_image_params={cv::IMWRITE_JPEG_QUALITY,50};
        std::vector<uint8_t> m_ui8_buffer;
        std::string m_s_rviz_plugin_namespace;
        float m_f_view_pause;
        float m_f_view_elapsed;
        float m_f_view_distance;
        float m_f_frames_rendered;
        int m_i_view_diff_x;
        int m_i_view_diff_y;
        bool m_b_mouse_drag_enabled;


    protected Q_SLOTS:

        // Inherited methods
        void onInitialize();
        void onActivate();
        void reset();
        void update(float dt,float ros_dt);
        void handleMouseEvent(rviz_common::ViewportMouseEvent& evt);
        void transitionFrom(ViewController* previous_view);
        void lookAt(const Ogre::Vector3& point);

        // Callbacks to process ROS2 messages
        void cb_sub_vtr_view_trajectory(rviz_plugin_view_animated_msgs::msg::ViewTrajectory msg_vtr_trajectory);
        void cb_sub_f32_view_pause(std_msgs::msg::Float32 msg_f32_pause);

        // Callbacks to process property updates
        void cb_prp_update_view_tf_frame();
        void cb_prp_update_view_eye_local();
        void cb_prp_update_view_focus_local();
        void cb_prp_update_view_up_local();
        void cb_prp_update_view_eye();
        void cb_prp_update_view_focus();
        void cb_prp_update_view_up();
        void cb_prp_update_view_up_lock_enabled();
        void cb_prp_update_view_distance();
        void cb_prp_update_view_direction();
        void cb_prp_update_view_orientation();
        void cb_prp_update_view_live_enable();
        void cb_prp_update_view_live_record_enable();
        void cb_prp_update_view_live_color();
        void cb_prp_update_view_live_topic();
        void cb_prp_update_view_pose_topic();
        void cb_prp_update_view_animation_completed_topic();
        void cb_prp_update_view_animation_pause_topic();
        void cb_prp_update_view_trajectory_topic();

        // Helper methods
        void UpdateViewTopicsAndTransports();
        void UpdateViewPose();
        void UpdateViewYawPitchRoll(float f_yaw,float f_pitch,float f_roll);
        void UpdateViewEye(float f_x,float f_y,float f_z);
        void UpdateViewEyeFocus(float f_x,float f_y,float f_z);
        void UpdateViewInitialize();
        void UpdateViewCancel();
        float CalculateViewAnimationIteration(float d_time_delta,uint8_t ui8_movement_mode);
        void OrbitViewTo(const Ogre::Vector3& ov3_view_eye);
        void MoveEyeWithFocusTo(const Ogre::Vector3& ov3_view_eye_focus);
        Ogre::Vector3 FixedFrameToAttachedLocal(const Ogre::Vector3& ov3_vector);
        Ogre::Vector3 AttachedLocalToFixedFrame(const Ogre::Vector3& ov3_vector);
        void TransformViewTrajectoryToFixedTFFrame(geometry_msgs::msg::PointStamped& msg_pos_eye,
                                          geometry_msgs::msg::PointStamped& msg_pos_focus,
                                          geometry_msgs::msg::Vector3Stamped& msg_v3s_up,bool b_is_fps=false);
        Ogre::Quaternion getQuaternionFromDirection(const Ogre::Vector3& ov3_direction,const Ogre::Vector3& ov3_up_vector);
        void InitializeViewLiveTexture();
        void UpdateViewLiveImageResolution();
        void PublishViewPose();
        void PublishViewAnimationCompleted();
        void PublishViewLiveImage();


    public:

        // Default constructor and destructor
        RVizPluginViewAnimated();
        ~RVizPluginViewAnimated();
    };
}  // namespace rviz_plugin_view_animated

#endif  // RVIZ_PLUGIN_VIEW_ANIMATED_HPP
