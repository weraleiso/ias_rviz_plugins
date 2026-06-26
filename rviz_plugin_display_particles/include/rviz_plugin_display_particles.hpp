#ifndef RVIZ_PLUGIN_DISPLAY_PARTICLES_HPP
#define RVIZ_PLUGIN_DISPLAY_PARTICLES_HPP

// ROS2 libraries
#include<rclcpp/rclcpp.hpp>
#include<rcl_interfaces/msg/parameter_event.hpp>
#include<ament_index_cpp/get_package_share_directory.hpp>

// OpenCV libraries
// ...

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
#include<OgreEntity.h>
#include<OgreCamera.h>
#include<OgreFrustum.h>
#include<OgreSceneManager.h>
#include<OgreSceneNode.h>
#include<OgreVector3.h>
#include<OgreQuaternion.h>
#include<OgreParticleSystem.h>
#include<OgreParticle.h>
#include<OgreParticleEmitter.h>
#include<OgreParticleAffector.h>

// STANDARD libraries
#include<boost/random.hpp>



namespace rviz_plugin_display_particles
{
    static int si_resource_name_count=0; // Make resource names unique

    class RVizPluginDisplayParticles:public rviz_common::Display
    {
        Q_OBJECT

        // ROS2 properties
        rclcpp::Node::SharedPtr m_hdl_node;
        std::shared_ptr<rviz_common::ros_integration::RosNodeAbstractionIface> m_hdl_node_rviz;

        // OpenCV properties
        // ...

        // RViz properties
        std::unique_ptr<rviz_common::properties::TfFrameProperty> m_prp_particles_tf_frame;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_particles_position;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_particles_direction;
        std::unique_ptr<rviz_common::properties::EnumProperty> m_prp_particles_preset;
        std::unique_ptr<rviz_common::properties::EnumProperty> m_prp_particles_type;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_particles_quota;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_particles_emission_rate;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_angle;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_size;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_particles_color_range_start;
        std::unique_ptr<rviz_common::properties::ColorProperty> m_prp_particles_color_range_end;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_time_to_live;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_velocity_max;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_particles_colour_fader;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_colour_fader_alpha;
        std::unique_ptr<rviz_common::properties::VectorProperty> m_prp_particles_linear_force;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_scaler;
        std::unique_ptr<rviz_common::properties::IntProperty> m_prp_particles_randomness;
        std::unique_ptr<rviz_common::properties::FloatProperty> m_prp_particles_scope;
        std::unique_ptr<rviz_common::properties::BoolProperty> m_prp_particles_keep_velocity;

        // OGRE properties
        Ogre::ParticleSystem* m_ogr_particlesystem;
        Ogre::Vector3 m_ov3_tf_frame_position;
        Ogre::Quaternion m_oqu_tf_frame_orientation;

        // Helper properties
        std::string m_s_rviz_plugin_namespace;


    protected Q_SLOTS:

        // Inherited methods
        void onInitialize();
        void onDisable();
        void onEnable();
        void reset();
        void update(float wall_dt,float ros_dt);

        // Callbacks to process ROS2 messages
        // ...

        // Callbacks to process property updates
        void cb_prp_update_particles_tf_frame();
        void cb_prp_update_particles_position();
        void cb_prp_update_particles_direction();
        void cb_prp_update_particles_preset();
        void cb_prp_update_particles_type();
        void cb_prp_update_particles_quota();
        void cb_prp_update_particles_emission_rate();
        void cb_prp_update_particles_angle();
        void cb_prp_update_particles_size();
        void cb_prp_update_particles_color();
        void cb_prp_update_particles_time_to_live();
        void cb_prp_update_particles_velocity_max();
        void cb_prp_update_particles_colour_fader();
        void cb_prp_update_particles_linear_force();
        void cb_prp_update_particles_scaler();
        void cb_prp_update_particles_randomness();

        // Helper methods
        void UpdateParticlesSandbox();
        void UpdateParticlesOgreResources();
        void UpdateParticlesPosition();
        void UpdateParticlesDirection();
        void UpdateParticlesPreset();
        void UpdateParticlesType();
        void UpdateParticlesQuota();
        void UpdateParticlesEmissionRate();
        void UpdateParticlesAngle();
        void UpdateParticlesSize();
        void UpdateParticlesColor();
        void UpdateParticlesTimeToLive();
        void UpdateParticlesVelocityMax();
        void UpdateParticlesColourFader();
        void UpdateParticlesLinearForce();
        void UpdateParticlesScaler();
        void UpdateParticlesRandomness();
        void UpdateParticlesPropertiesAll();


    public:

        // Default constructor and destructor
        RVizPluginDisplayParticles();
        ~RVizPluginDisplayParticles();
    };
}  // namespace rviz_plugin_display_particles

#endif  // RVIZ_PLUGIN_DISPLAY_PARTICLES_HPP
