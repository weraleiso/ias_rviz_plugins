#ifndef RVIZ_PLUGIN_DISPLAY_PROJECTOR_HPP
#define RVIZ_PLUGIN_DISPLAY_PROJECTOR_HPP

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



namespace rviz_plugin_display_projector
{
    class RVizPluginDisplayProjector:public rviz_common::Display
    {
        Q_OBJECT

        // ROS2 properties
        rclcpp::Node::SharedPtr m_hdl_node;
        std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> m_hdl_node_rviz;
        std::shared_ptr<image_transport::ImageTransport> m_hdl_it;
        image_transport::Subscriber m_sub_img_projector;
        tf2::Transform m_tf2_projector;

        // OpenCV properties
        cv::Mat m_mat_img_projector;

        // RViz properties
        std::unique_ptr<rviz_common::properties::RosFilteredTopicProperty> m_prp_projector_image_topic;
        std::unique_ptr<rviz_common::properties::StringProperty> m_prp_projector_topic;
        std::unique_ptr<rviz_common::properties::EnumProperty> m_prp_projector_transport;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_projector_res_x;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_projector_res_y;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_projector_width;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_projector_height;
        std::unique_ptr<rviz_common::properties::TfFrameProperty> m_prp_projector_tf_frame;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_projector_position;
        std::unique_ptr<rviz_common::properties::QuaternionProperty> m_prp_projector_orientation;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_projector_tf_refresh;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_projector_border_thickness;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_projector_border_color;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_projector_alpha;
        std::unique_ptr<rviz_rendering::Shape> m_shp_projector_border_left;
        std::unique_ptr<rviz_rendering::Shape> m_shp_projector_border_right;
        std::unique_ptr<rviz_rendering::Shape> m_shp_projector_border_top;
        std::unique_ptr<rviz_rendering::Shape> m_shp_projector_border_bottom;

        // OGRE properties
        Ogre::SceneNode* m_hdl_node_scene;
        Ogre::TexturePtr m_tep_texture;
        Ogre::MaterialPtr m_map_material;
        Ogre::TextureUnitState* m_tus_texture_unit_state;
        Ogre::Pass* m_pas_texture;
        Ogre::Entity* m_ent_entity;
        Ogre::Plane m_pla_plane;
        Ogre::HardwarePixelBufferSharedPtr m_hpb_buffer;
        uint8_t* m_ui8_buffer;
        Ogre::Vector3 m_ov3_position;
        Ogre::Quaternion m_oqu_orientation;

        // Helper properties
        static int si_resource_name_count; // Make resource names unique
        std::string m_s_rviz_plugin_namespace;
        int m_i_projector_res_x;
        int m_i_projector_res_y;


    protected Q_SLOTS:

        // Inherited methods
        void onInitialize();
        void onDisable();
        void onEnable();
        void reset();
        void update(float wall_dt,float ros_dt);

        // Callbacks to process ROS2 messages
        void cb_sub_img_projector(const sensor_msgs::msg::Image::ConstSharedPtr& msg_img_projector);

        // Callbacks to process property updates
        void cb_prp_update_projector_topics();
        void cb_prp_update_projector_transports();
        void cb_prp_update_projector_res();
        void cb_prp_update_projector_width();
        void cb_prp_update_projector_height();
        void cb_prp_update_projector_tf_frame();
        void cb_prp_update_projector_position();
        void cb_prp_update_projector_orientation();
        void cb_prp_update_projector_tf_refresh();
        void cb_prp_update_projector_border_thickness();
        void cb_prp_update_projector_border_color();
        void cb_prp_update_projector_alpha();

        // Helper methods
        void UpdateProjectorOgreResources();
        void UpdateProjectorOgreTexture();
        void UpdateProjectorOgreTextureResolution(int i_res_x,int i_res_y);
        void UpdateProjectorTopicsAndTransports();
        void UpdateProjectorTFFrame();
        void UpdateProjectorPose();
        void UpdateProjectorBorder();
        void UpdateProjectorAlpha();


    public:

        // Default constructor and destructor
        RVizPluginDisplayProjector();
        ~RVizPluginDisplayProjector();
    };
}  // namespace rviz_plugin_display_projector

#endif  // RVIZ_PLUGIN_DISPLAY_PROJECTOR_HPP
