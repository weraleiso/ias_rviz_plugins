#ifndef RVIZ_PLUGIN_DISPLAY_LIGHTING_HPP
#define RVIZ_PLUGIN_DISPLAY_LIGHTING_HPP

// ROS2 libraries
#include<rclcpp/rclcpp.hpp>
#include<rcl_interfaces/msg/parameter_event.hpp>
#include<ament_index_cpp/get_package_share_directory.hpp>
#include<std_msgs/msg/bool.hpp>
#include<std_msgs/msg/color_rgba.hpp>
#include<visualization_msgs/msg/marker.hpp>

// OpenCV libraries
// ...

// RViz libraries
#include<rviz_common/ros_integration/ros_node_abstraction_iface.hpp>
#include<rviz_common/message_filter_display.hpp>
#include<rviz_common/logging.hpp>
#include<rviz_common/display_context.hpp>
#include<rviz_common/frame_manager_iface.hpp>
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



namespace rviz_plugin_display_lighting
{
    class RVizPluginDisplayLighting:public rviz_common::Display
    {
        Q_OBJECT

        // ROS2 properties
        rclcpp::Node::SharedPtr m_hdl_node;
        std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> m_hdl_node_rviz;
        rclcpp::Subscription<std_msgs::msg::ColorRGBA>::SharedPtr m_sub_col_light_color_ambient;
        rclcpp::Subscription<std_msgs::msg::ColorRGBA>::SharedPtr m_sub_col_light_color_diff_spec;
        rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr m_sub_bol_light_enable;
        rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr m_pub_mrk_light;
        visualization_msgs::msg::Marker m_msg_mrk_pointlight;
        visualization_msgs::msg::Marker m_msg_mrk_spotlight;
        visualization_msgs::msg::Marker m_msg_mrk_sunlight;
        geometry_msgs::msg::Pose m_pos_mrk_pose;
        tf2::Quaternion m_qua_direction;
        tf2::Transform m_tf2_global_wrt_local;
        tf2::Transform m_tf2_local_wrt_lighting;
        tf2::Transform m_tf2_global;

        // OpenCV properties
        // ...

        // RViz properties
        std::unique_ptr<rviz_common::properties::TfFrameProperty> m_prp_lighting_tf_frame;
        std::unique_ptr<rviz_common::properties::EnumProperty> m_prp_lighting_type;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_lighting_shadows_enabled;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_lighting_marker_enabled;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_lighting_color_ambient;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_lighting_color_diffuse;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_lighting_color_specular;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_lighting_color_diff_spec_hsv;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_lighting_color_diff_spec_ycbcr;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_lighting_position;
        std::unique_ptr<rviz_common::properties::QuaternionProperty> m_prp_lighting_orientation;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_lighting_direction;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_lighting_range;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_lighting_attenuation;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_lighting_angle_inner;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_lighting_angle_outer;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_lighting_falloff_rate;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_lighting_sun_distance;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_lighting_sun_rise_hour;
        std::unique_ptr<rviz_common::properties::StringProperty> m_prp_lighting_marker_path;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_lighting_topic_marker;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_lighting_topic_color_ambient;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_lighting_topic_color_diff_spec;
        std::unique_ptr<rviz_common::properties::RosTopicProperty> m_prp_lighting_topic_enable;

        // OGRE properties
        Ogre::Light* m_ogr_light_rviz_default;
        Ogre::Light* m_ogr_light_plugin;
        Ogre::Vector3 m_ov3_tf_frame_position;
        Ogre::Quaternion m_oqu_tf_frame_orientation;
        Ogre::Vector3 m_ov3_lighting_position;
        Ogre::Quaternion m_oqu_lighting_orientation;

        // Helper properties
        static int si_resource_name_count; // Make resource names unique
        std::string m_s_rviz_plugin_namespace;


    protected Q_SLOTS:

        // Inherited methods
        void onInitialize();
        void onDisable();
        void onEnable();
        void reset();
        void update(float wall_dt,float ros_dt);

        // Callbacks to process ROS2 messages
        void cb_sub_col_lighting_color_ambient(std_msgs::msg::ColorRGBA msg_col_ambient);
        void cb_sub_col_lighting_color_diff_spec(std_msgs::msg::ColorRGBA msg_col_diff_spec);
        void cb_sub_bol_lighting_enable(std_msgs::msg::Bool msg_bol_enable);

        // Callbacks to process property updates
        void cb_prp_update_lighting_tf_frame();
        void cb_prp_update_lighting_type();
        void cb_prp_update_lighting_shadows_enabled();
        void cb_prp_update_lighting_marker_enabled();
        void cb_prp_update_lighting_position();
        void cb_prp_update_lighting_orientation();
        void cb_prp_update_lighting_direction();
        void cb_prp_update_lighting_color_ambient();
        void cb_prp_update_lighting_color_diffuse();
        void cb_prp_update_lighting_color_specular();
        void cb_prp_update_lighting_range();
        void cb_prp_update_lighting_attenuation();
        void cb_prp_update_lighting_angle_inner();
        void cb_prp_update_lighting_angle_outer();
        void cb_prp_update_lighting_falloff_rate();
        void cb_prp_update_lighting_sunlight();
        void cb_prp_update_lighting_marker_path();
        void cb_prp_update_light_marker_topic();
        void cb_prp_update_light_color_ambient_topic();
        void cb_prp_update_light_color_diff_spec_topic();
        void cb_prp_update_light_enable_topic();

        // Helper methods
        void UpdateLightingOgreResources();
        void UpdateLightingTopicsAndTransports();
        void UpdateLightingType();
        void UpdateLightingColors();
        void UpdateLightingColorsHSV();
        void UpdateLightingColorsYCbCr();
        void UpdateLightingShadows();
        void UpdateLightingPose();
        void UpdateLightingDetails();
        void InitializeLightingMarker();
        void UpdateLightingMarker();
        void DisableLightingMarker();
        tf2::Quaternion getQuaternionFromDirection(tf2::Vector3 vc3_input);
        Ogre::Vector3 getVector3FromQuaternion(Ogre::Quaternion oqu_input);


    public:

        // Default constructor and destructor
        RVizPluginDisplayLighting();
        ~RVizPluginDisplayLighting();
    };
}  // namespace rviz_plugin_display_lighting

#endif  // RVIZ_PLUGIN_DISPLAY_LIGHTING_HPP
