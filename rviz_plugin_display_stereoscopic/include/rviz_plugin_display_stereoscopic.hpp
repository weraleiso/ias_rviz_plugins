#ifndef RVIZ_PLUGIN_DISPLAY_STEREOSCOPIC_HPP
#define RVIZ_PLUGIN_DISPLAY_STEREOSCOPIC_HPP

// ROS2 libraries
#include<rclcpp/rclcpp.hpp>
#include<rcl_interfaces/msg/parameter_event.hpp>
#include<ament_index_cpp/get_package_share_directory.hpp>
#include<image_transport/image_transport.hpp>
#include<cv_bridge/cv_bridge.hpp>
#include<sensor_msgs/msg/image.hpp>
#include<sensor_msgs/image_encodings.hpp>

// OpenCV libraries
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/calib3d.hpp>

// RViz libraries
#include<rviz_common/ros_integration/ros_node_abstraction_iface.hpp>
#include<rviz_common/message_filter_display.hpp>
#include<rviz_common/logging.hpp>
#include<rviz_common/display_context.hpp>
#include<rviz_common/frame_manager_iface.hpp>
#include<rviz_common/properties/ros_topic_property.hpp>
#include<rviz_common/properties/bool_property.hpp>
#include<rviz_common/properties/int_property.hpp>
#include<rviz_common/properties/enum_property.hpp>
#include<rviz_common/properties/float_property.hpp>
#include<rviz_common/properties/string_property.hpp>
#include<rviz_common/properties/ros_topic_property.hpp>
#include<rviz_common/properties/tf_frame_property.hpp>
#include<rviz_common/properties/vector_property.hpp>
#include<rviz_common/properties/quaternion_property.hpp>
#include<rviz_common/properties/color_property.hpp>
#include<rviz_rendering/objects/shape.hpp>

// OGRE libraries
#include<Ogre.h>

// STANDARD libraries
#include<chrono>
#include<memory>
#include<string>



namespace rviz_plugin_display_stereoscopic
{
    class RVizPluginDisplayStereoscopic:public rviz_common::Display
    {
        Q_OBJECT

        // ROS2 properties
        rclcpp::Node::SharedPtr m_hdl_node;
        std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> m_hdl_node_rviz;
        std::shared_ptr<image_transport::ImageTransport> m_hdl_it;
        image_transport::Subscriber m_sub_stereoscopic_camera_pose;
        image_transport::Publisher m_pub_img_stereoscopic_portable;
        tf2::Transform m_tf2_projector;
        rclcpp::TimerBase::SharedPtr tmr_init_delayed;
        std::shared_ptr<sensor_msgs::msg::Image> m_msg_img_stereoscopic_portable;

        // OpenCV properties
        cv::Mat* mat_img_stereoscopic_eye_left;
        cv::Mat* mat_img_stereoscopic_eye_left_dist;
        cv::Mat* mat_img_stereoscopic_eye_right;
        cv::Mat* mat_img_stereoscopic_eye_right_dist;
        cv::Mat* mat_img_stereoscopic_portable;
        cv::Mat m_mat_camera_matrix;
        cv::Mat m_mat_distortion_coefficients;
        cv::Mat m_mat_undistort_map_1;
        cv::Mat m_mat_undistort_map_2;

        // RViz properties       
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_stereoscopic_view_image_topic;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_stereoscopic_view_res_x;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_stereoscopic_view_res_y;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_stereoscopic_ipd;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_stereoscopic_dist;

        // OGRE properties
        Ogre::SceneManager* m_ogr_scene_manager;
        Ogre::SceneNode* m_ogr_scene_node;
        Ogre::Camera* m_ogr_camera_current;
        Ogre::Camera* m_ogr_camera_eye_left;
        Ogre::Camera* m_ogr_camera_eye_right;
        Ogre::TexturePtr m_ogr_texture_ptr_eye_left;
        Ogre::TexturePtr m_ogr_texture_ptr_eye_right;
        Ogre::RenderTexture* m_ogr_render_texture_eye_left;
        Ogre::RenderTexture* m_ogr_render_texture_eye_right;
        Ogre::Vector3 m_ov3_camera_position;
        Ogre::Quaternion m_oqu_camera_orientation;

        // Helper properties
        static int si_resource_name_count; // Make resource names unique
        std::string m_s_rviz_plugin_namespace;
        int m_i_stereoscopic_view_res_x;
        int m_i_stereoscopic_view_res_y;
        bool m_b_ogr_camera_initialized;


    protected Q_SLOTS:

        // Inherited methods
        void onInitialize();
        void onDisable();
        void onEnable();
        void reset();
        void update(float wall_dt,float ros_dt);

        // Callbacks to process ROS2 messages
        void cb_tmr_init_delayed();

        // Callbacks to process property updates
        void cb_prp_update_stereoscopic_topics();
        void cb_prp_update_stereoscopic_resolution();
        void cb_prp_update_stereoscopic_ipd();
        void cb_prp_update_stereoscopic_distortion();

        // Helper methods
        void CleanupStereoscopicTexture();
        void InitStereoscopicTexture();
        void UpdateStereoscopicTopicsAndTransports();
        void UpdateStereoscopicResultion();
        void UpdateStereoscopicIntrinsics();
        void UpdateStereoscopicExtrinsics();
        void UpdateStereoscopicView();


    public:

        // Default constructor and destructor
        RVizPluginDisplayStereoscopic();
        ~RVizPluginDisplayStereoscopic();
    };
}  // namespace rviz_plugin_display_stereoscopic

#endif  // RVIZ_PLUGIN_DISPLAY_STEREOSCOPIC_HPP
