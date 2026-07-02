#include<rviz_plugin_display_lighting.hpp>



namespace rviz_plugin_display_lighting
{
    // Inherited methods
    void RVizPluginDisplayLighting::onInitialize()
    {
        // Initialize ROS2 properties
        m_hdl_node_rviz=context_->getRosNodeAbstraction().lock();
        m_hdl_node=m_hdl_node_rviz->get_raw_node();

        // Initialize helper properties
        si_resource_name_count++;
        m_s_rviz_plugin_namespace="rviz_plugin_display_lighting_"+std::to_string(si_resource_name_count)+"/";

        // Initialize RViz properties
        m_prp_lighting_tf_frame=std::make_unique<rviz_common::properties::TfFrameProperty>("TF Frame",rviz_common::properties::TfFrameProperty::FIXED_FRAME_STRING,"TF Frame.",this,(rviz_common::FrameManager*)context_->getFrameManager(),true,SLOT(cb_prp_update_lighting_tf_frame()));
        m_prp_lighting_type=std::make_unique<rviz_common::properties::EnumProperty>("Type","Ambient","Type.",this,SLOT(cb_prp_update_lighting_type()));
        m_prp_lighting_type->addOption("Ambient",3);
        m_prp_lighting_type->addOption("Directional",Ogre::Light::LT_DIRECTIONAL);
        m_prp_lighting_type->addOption("Point",Ogre::Light::LT_POINT);
        m_prp_lighting_type->addOption("Spot",Ogre::Light::LT_SPOTLIGHT);
        m_prp_lighting_type->addOption("Sun",4);
        m_prp_lighting_shadows_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Shadows Enabled",false,"Shadows Enabled.",this,SLOT(cb_prp_update_lighting_shadows_enabled()));
        m_prp_lighting_marker_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Marker Enabled",true,"Marker Enabled.",this,SLOT(cb_prp_update_lighting_marker_enabled()));
        m_prp_lighting_color_ambient=std::make_unique<rviz_common::properties::ColorProperty>("Ambient Color",QColor(119,118,123,255),"Ambient Color.",this,SLOT(cb_prp_update_lighting_color_ambient()));
        m_prp_lighting_color_diffuse=std::make_unique<rviz_common::properties::ColorProperty>("Diffuse Color",QColor(119,118,123,255),"Diffuse Color.",this,SLOT(cb_prp_update_lighting_color_diffuse()));
        m_prp_lighting_color_specular=std::make_unique<rviz_common::properties::ColorProperty>("Specular Color",QColor(119,118,123,255),"Specular Color.",this,SLOT(cb_prp_update_lighting_color_specular()));
        m_prp_lighting_color_diff_spec_hsv=std::make_unique<rviz_common::properties::VectorProperty>("Diffuse/Specular HSV",Ogre::Vector3::ZERO,"Diffuse/Specular HSV.",this,SLOT(UpdateLightingColorsHSV()));
        m_prp_lighting_color_diff_spec_ycbcr=std::make_unique<rviz_common::properties::VectorProperty>("Diffuse/Specular YCbCr",Ogre::Vector3::ZERO,"Diffuse/Specular YCbCr.",this,SLOT(UpdateLightingColorsYCbCr()));
        m_prp_lighting_position=std::make_unique<rviz_common::properties::VectorProperty>("Position",Ogre::Vector3(1.0,1.0,1.0),"Position.",this,SLOT(cb_prp_update_lighting_position()));
        m_prp_lighting_orientation=std::make_unique<rviz_common::properties::QuaternionProperty>("Orientation",Ogre::Quaternion(1.0,0.0,0.0,0.0),"Orientation.",this,SLOT(cb_prp_update_lighting_orientation()));
        m_prp_lighting_orientation->setReadOnly(true);
        m_prp_lighting_direction=std::make_unique<rviz_common::properties::VectorProperty>("Direction",Ogre::Vector3(-1.0,-1.0,-1.0),"Direction.",this,SLOT(cb_prp_update_lighting_direction()));
        m_prp_lighting_range=std::make_unique<rviz_common::properties::FloatProperty>("Range",100.0,"Range.",this,SLOT(cb_prp_update_lighting_range()));
        m_prp_lighting_attenuation=std::make_unique<rviz_common::properties::VectorProperty>("Attenuation",Ogre::Vector3(1.0,0.0,0.0),"Attenuation.",this,SLOT(cb_prp_update_lighting_attenuation()));
        m_prp_lighting_angle_inner=std::make_unique<rviz_common::properties::FloatProperty>("Inner Cone Angle",1.0,"Inner Cone Angle.",this,SLOT(cb_prp_update_lighting_angle_inner()));
        m_prp_lighting_angle_outer=std::make_unique<rviz_common::properties::FloatProperty>("Outer Cone Angle",2.0,"Outer Cone Angle.",this,SLOT(cb_prp_update_lighting_angle_outer()));
        m_prp_lighting_falloff_rate=std::make_unique<rviz_common::properties::FloatProperty>("Falloff Rate",50.0,"Falloff Rate.",this,SLOT(cb_prp_update_lighting_falloff_rate()));
        m_prp_lighting_sun_distance=std::make_unique<rviz_common::properties::FloatProperty>("Sun Distance",10.0,"Sun Distance.",this,SLOT(cb_prp_update_lighting_sunlight()));
        m_prp_lighting_sun_rise_hour=std::make_unique<rviz_common::properties::IntProperty>("Sun Rise Hour",6,"Sun Rise Hour.",this,SLOT(cb_prp_update_lighting_sunlight()));
        m_prp_lighting_marker_path=std::make_unique<rviz_common::properties::StringProperty>("Marker Path","/home/ias/ros2_ws/src/ias_wor_interactivespace/ias_wor_universe/ias_mod_universe/ias_rviz_plugins/rviz_plugin_display_lighting/resources/","Marker Path.",this,SLOT(cb_prp_update_lighting_marker_path()));
        m_prp_lighting_topic_marker=std::make_unique<rviz_common::properties::RosTopicProperty>("Marker Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"marker"),"visualization_msgs/msg/Marker","Marker Topic.",this,SLOT(cb_prp_update_light_marker_topic()));
        m_prp_lighting_topic_marker->initialize(m_hdl_node_rviz);
        m_prp_lighting_topic_color_ambient=std::make_unique<rviz_common::properties::RosTopicProperty>("Color Ambient Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"color_ambient"),"std_msgs/msg/ColorRGBA","Color Ambient Topic.",this,SLOT(cb_prp_update_light_color_ambient_topic()));
        m_prp_lighting_topic_color_ambient->initialize(m_hdl_node_rviz);
        m_prp_lighting_topic_color_diff_spec=std::make_unique<rviz_common::properties::RosTopicProperty>("Diffuse/Specular Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"color_diff_sepc"),"std_msgs/msg/ColorRGBA","Color Diff./Spec. Topic.",this,SLOT(cb_prp_update_light_color_diff_spec_topic()));
        m_prp_lighting_topic_color_diff_spec->initialize(m_hdl_node_rviz);
        m_prp_lighting_topic_enable=std::make_unique<rviz_common::properties::RosTopicProperty>("Enable Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"enable"),"std_msgs/msg/Bool","Enable Topic.",this,SLOT(cb_prp_update_light_enable_topic()));
        m_prp_lighting_topic_enable->initialize(m_hdl_node_rviz);

        // Initialize OGRE properties
        UpdateLightingOgreResources();

        // Initialize ROS2 subscribers and publishers
        UpdateLightingType();
        UpdateLightingTopicsAndTransports();
        InitializeLightingMarker();

        setStatusStd(rviz_common::properties::StatusProperty::Ok,"Lighting","Ok!");
        RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_display_lighting::RVizPluginDisplayLighting - Initialized!");
    }
    void RVizPluginDisplayLighting::onDisable()
    {
        context_->getSceneManager()->setAmbientLight(Ogre::ColourValue(0.0,0.0,0.0,0.0));
        m_ogr_light_plugin->setVisible(false);
        m_ogr_light_rviz_default->setVisible(true);
        // context_->getSceneManager()->destroyLight(m_ogr_light_plugin);
        DisableLightingMarker();
    }
    void RVizPluginDisplayLighting::onEnable()
    {
        m_ogr_light_rviz_default->setVisible(false);
        UpdateLightingType();
    }
    void RVizPluginDisplayLighting::reset()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::update(float wall_dt,float ros_dt)
    {
        if(context_->getFrameManager()->getTransform(m_prp_lighting_tf_frame->getFrameStd(),m_ov3_tf_frame_position,m_oqu_tf_frame_orientation))
        {
            UpdateLightingPose();
            if(m_prp_lighting_marker_enabled->getBool()==true)
            {
                UpdateLightingMarker();
            }
            setStatusStd(rviz_common::properties::StatusProperty::Ok,"Lighting","Ok!");
        }
        else
        {
            setStatusStd(rviz_common::properties::StatusProperty::Warn,"Lighting","No valid transform!");
        }
    }

    // Callbacks to process ROS2 messages
    void RVizPluginDisplayLighting::cb_sub_col_lighting_color_ambient(std_msgs::msg::ColorRGBA msg_col_ambient)
    {
        m_prp_lighting_color_ambient->setColor(QColor(msg_col_ambient.r,msg_col_ambient.g,msg_col_ambient.b,msg_col_ambient.a));
    }
    void RVizPluginDisplayLighting::cb_sub_col_lighting_color_diff_spec(std_msgs::msg::ColorRGBA msg_col_diff_spec)
    {
        m_prp_lighting_color_diffuse->setColor(QColor(msg_col_diff_spec.r,msg_col_diff_spec.g,msg_col_diff_spec.b,255));
        m_prp_lighting_color_specular->setColor(QColor(msg_col_diff_spec.r,msg_col_diff_spec.g,msg_col_diff_spec.b,255));
    }
    void RVizPluginDisplayLighting::cb_sub_bol_lighting_enable(std_msgs::msg::Bool msg_bol_enable)
    {
        if(msg_bol_enable.data==true)
        {
            onEnable();
        }
        else
        {
            onDisable();
        }
    }

    // Callbacks to process property updates
    void RVizPluginDisplayLighting::cb_prp_update_lighting_tf_frame()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_type()
    {
        UpdateLightingType();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_shadows_enabled()
    {
        // Important note:
        // Enabling shadows may break ray-casting for point-tool selection!
        // After re-enabling, the shadows would need to be re-DISABLED for each and every material!
        UpdateLightingShadows();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_marker_enabled()
    {
        if(m_prp_lighting_marker_enabled->getBool()==false)
        {
            DisableLightingMarker();
        }
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_position()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_orientation()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_direction()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_color_ambient()
    {
        UpdateLightingColors();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_color_diffuse()
    {
        UpdateLightingColors();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_color_specular()
    {
        UpdateLightingColors();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_range()
    {
        UpdateLightingDetails();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_attenuation()
    {
        UpdateLightingDetails();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_angle_inner()
    {
        UpdateLightingDetails();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_angle_outer()
    {
        UpdateLightingDetails();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_falloff_rate()
    {
        UpdateLightingDetails();
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_sunlight()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_lighting_marker_path()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_light_marker_topic()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_light_color_ambient_topic()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_light_color_diff_spec_topic()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayLighting::cb_prp_update_light_enable_topic()
    {
        // Do nothing for now...
    }

    // Helper methods
    void RVizPluginDisplayLighting::UpdateLightingOgreResources()
    {
        m_ogr_light_rviz_default=context_->getSceneManager()->getLight("MainDirectional");
        m_ogr_light_plugin=context_->getSceneManager()->createLight();
        m_qua_direction.setValue(0.0,0.0,0.0,1.0);
        m_tf2_global.setOrigin(tf2::Vector3(0.0,0.0,0.0));
        m_tf2_global.setRotation(tf2::Quaternion(0.0,0.0,0.0,1.0));
        m_tf2_global_wrt_local=m_tf2_global;
        m_tf2_local_wrt_lighting=m_tf2_global;
        m_ov3_tf_frame_position=Ogre::Vector3(0.0,0.0,0.0);
        m_oqu_tf_frame_orientation=Ogre::Quaternion(1.0,0.0,0.0,0.0);
    }
    void RVizPluginDisplayLighting::UpdateLightingTopicsAndTransports()
    {
        m_sub_col_light_color_ambient.reset();
        m_sub_col_light_color_ambient=m_hdl_node->create_subscription<std_msgs::msg::ColorRGBA>(
                                    m_prp_lighting_topic_color_ambient->getStdString(),
                                    1,
                                    std::bind(&RVizPluginDisplayLighting::cb_sub_col_lighting_color_ambient,this,std::placeholders::_1));
        m_sub_col_light_color_diff_spec.reset();
        m_sub_col_light_color_diff_spec=m_hdl_node->create_subscription<std_msgs::msg::ColorRGBA>(
                                    m_prp_lighting_topic_color_diff_spec->getStdString(),
                                    1,
                                    std::bind(&RVizPluginDisplayLighting::cb_sub_col_lighting_color_diff_spec,this,std::placeholders::_1));
        m_sub_bol_light_enable.reset();
        m_sub_bol_light_enable=m_hdl_node->create_subscription<std_msgs::msg::Bool>(
                                    m_prp_lighting_topic_enable->getStdString(),
                                    1,
                                    std::bind(&RVizPluginDisplayLighting::cb_sub_bol_lighting_enable,this,std::placeholders::_1));
        m_pub_mrk_light.reset();
        m_pub_mrk_light=m_hdl_node->create_publisher<visualization_msgs::msg::Marker>(
                                    m_prp_lighting_topic_marker->getStdString(),
                                    1);
    }
    void RVizPluginDisplayLighting::UpdateLightingType()
    {
        m_ogr_light_plugin->setVisible(false);
        switch(m_prp_lighting_type->getOptionInt())
        {
            case Ogre::Light::LT_POINT: // (0) POINT LIGHT
                m_ogr_light_plugin->setType(Ogre::Light::LT_POINT);
            break;

            case Ogre::Light::LT_DIRECTIONAL: // (1) DIRECTIONAL LIGHT
                m_ogr_light_plugin->setType(Ogre::Light::LT_DIRECTIONAL);
            break;

            case Ogre::Light::LT_SPOTLIGHT: // (2) SPOTLIGHT LIGHT
                m_ogr_light_plugin->setType(Ogre::Light::LT_SPOTLIGHT);
            break;

            case 3: // (3) AMBIENT LIGHT
                context_->getSceneManager()->setAmbientLight(m_prp_lighting_color_ambient->getOgreColor());
                m_ogr_light_plugin->setDiffuseColour(m_prp_lighting_color_diffuse->getOgreColor());
                m_ogr_light_plugin->setSpecularColour(m_prp_lighting_color_specular->getOgreColor());
                m_ogr_light_plugin->setVisible(false);
                return;
                // Do nothing for now...
            break;

            case 4: // (4) SUNLIGHT (TIME DEPENDENT POINT LIGHT)
                m_ogr_light_plugin->setType(Ogre::Light::LT_POINT);
            break;

        default:
                // Do nothing...
            break;
        }

        // Update lighting
        UpdateLightingPose();
        UpdateLightingColors();
        UpdateLightingDetails();
        m_ogr_light_plugin->setVisible(true);
    }
    void RVizPluginDisplayLighting::UpdateLightingColors()
    {
        context_->getSceneManager()->setAmbientLight(m_prp_lighting_color_ambient->getOgreColor());
        m_ogr_light_plugin->setDiffuseColour(m_prp_lighting_color_diffuse->getOgreColor());
        m_ogr_light_plugin->setSpecularColour(m_prp_lighting_color_specular->getOgreColor());
    }
    void RVizPluginDisplayLighting::UpdateLightingColorsHSV()
    {
        /*
        Color schemes are:
        RGB ... classic red/green/blue color scheme (RGB Cube)
        HSB/HSV ... color scheme defined by Hue, Saturation and Brightness/Value (HSB/HSV Cylinder)
        HSL ... color scheme that is similar to HSB/HSV, however has a different color range (HSL Bi-Cone)
        Also check: https://medium.com/innovaccer-design/rgb-vs-hsb-vs-hsl-demystified-1992d7273d3a
        HSB/HSV color scheme: Hue determines type of color, Saturation is the color intensity, Value is representative for the brightness.
        [0 to 255] is equivalent to [0 to 360 deg] for the color circle, and equivalent to [0.0 to +1.0] for the saturation and value.
        YCbCr color scheme: According to JPEG standard Y′, Cb and Cr have the full 8-bit range.
        [0 to 255] is equivalent to [-1.0 to +1.0] in the CbCr plane.
        */
        Ogre::ColourValue ocl_lighting_diff_spec_hsv;
        ocl_lighting_diff_spec_hsv.setHSB(m_prp_lighting_color_diff_spec_hsv->getVector().x/255.0,
                                          m_prp_lighting_color_diff_spec_hsv->getVector().y/255.0,
                                          m_prp_lighting_color_diff_spec_hsv->getVector().z/255.0);
        m_prp_lighting_color_diffuse->setColor(QColor(ocl_lighting_diff_spec_hsv.r*255.0,
                                                      ocl_lighting_diff_spec_hsv.g*255.0,
                                                      ocl_lighting_diff_spec_hsv.b*255.0,
                                                      255));
        m_prp_lighting_color_specular->setColor(QColor(ocl_lighting_diff_spec_hsv.r*255.0,
                                                       ocl_lighting_diff_spec_hsv.g*255.0,
                                                       ocl_lighting_diff_spec_hsv.b*255.0,
                                                       255));
        UpdateLightingColors();
    }
    void RVizPluginDisplayLighting::UpdateLightingColorsYCbCr()
    {
        /*
        Color schemes are:
        RGB ... classic red/green/blue color scheme (RGB Cube)
        HSB/HSV ... color scheme defined by Hue, Saturation and Brightness/Value (HSB/HSV Cylinder)
        HSL ... color scheme that is similar to HSB/HSV, however has a different color range (HSL Bi-Cone)
        Also check: https://medium.com/innovaccer-design/rgb-vs-hsb-vs-hsl-demystified-1992d7273d3a
        HSB/HSV color scheme: Hue determines type of color, Saturation is the color intensity, Value is representative for the brightness.
        [0 to 255] is equivalent to [0 to 360 deg] for the color circle, and equivalent to [0.0 to +1.0] for the saturation and value.
        YCbCr color scheme: According to JPEG standard Y′, Cb and Cr have the full 8-bit range.
        [0 to 255] is equivalent to [-1.0 to +1.0] in the CbCr plane.
        */
        tf2::Vector3 tf_vc3_light_diff_spec_ycbcr(m_prp_lighting_color_diff_spec_ycbcr->getVector().x,  // Y'
                                                  m_prp_lighting_color_diff_spec_ycbcr->getVector().y,  // Cb
                                                  m_prp_lighting_color_diff_spec_ycbcr->getVector().z); // Cr
        tf2::Vector3 tf_vc3_light_diff_spec_rgb;
        tf_vc3_light_diff_spec_rgb.setX(tf_vc3_light_diff_spec_ycbcr.getX()+1.402*(tf_vc3_light_diff_spec_ycbcr.getZ()-128.0));
        tf_vc3_light_diff_spec_rgb.setY(tf_vc3_light_diff_spec_ycbcr.getX()-0.344136*(tf_vc3_light_diff_spec_ycbcr.getY()-128.0)-0.714136*(tf_vc3_light_diff_spec_ycbcr.getZ()-128.0));
        tf_vc3_light_diff_spec_rgb.setZ(tf_vc3_light_diff_spec_ycbcr.getX()+1.772*(tf_vc3_light_diff_spec_ycbcr.getY()-128.0));
        QColor qcl_light_diff_spec_rgb(tf_vc3_light_diff_spec_rgb.getX(),
                                       tf_vc3_light_diff_spec_rgb.getY(),
                                       tf_vc3_light_diff_spec_rgb.getZ(),
                                       255);
        m_prp_lighting_color_diffuse->setColor(qcl_light_diff_spec_rgb);
        m_prp_lighting_color_specular->setColor(qcl_light_diff_spec_rgb);
        UpdateLightingColors();
    }
    void RVizPluginDisplayLighting::UpdateLightingShadows()
    {
        /* Currently there are issues with the OGRE vendor library!
        // Also see following issue with Gazebo:
        // https://github.com/gazebosim/gazebo-classic/issues/2700
        if(m_prp_lighting_shadows_enabled->getBool()==true) context_->getSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
        else context_->getSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
        m_ogr_light_plugin->setCastShadows(m_prp_lighting_shadows_enabled->getBool());
        */
        setStatusStd(rviz_common::properties::StatusProperty::Warn,"Lighting","Currently not supported by OGRE vendor library!");
    }
    void RVizPluginDisplayLighting::UpdateLightingPose()
    {
        // Transform local lighting coordinates into selected global frame
        m_tf2_global_wrt_local.setOrigin(tf2::Vector3(m_ov3_tf_frame_position.x,m_ov3_tf_frame_position.y,m_ov3_tf_frame_position.z));
        m_tf2_global_wrt_local.setRotation(tf2::Quaternion(m_oqu_tf_frame_orientation.x,m_oqu_tf_frame_orientation.y,m_oqu_tf_frame_orientation.z,m_oqu_tf_frame_orientation.w));

        if(m_prp_lighting_type->getOptionInt()==4)
        {
            // Update pose of sunlight:
            // Scale angle of one full circle (2*M_PI rad) to 1440minutes of one day. At sunrise hour, angle of sunlight is 0.0 rad:
            std::time_t time_now; time(&time_now);
            struct tm tm_now=*std::localtime(&time_now);
            tf2::Vector3 vc3_local(tf2::Vector3(m_prp_lighting_position->getVector().x+0.0,
                                                m_prp_lighting_position->getVector().y-m_prp_lighting_sun_distance->getFloat()*cos( (float(tm_now.tm_hour-m_prp_lighting_sun_rise_hour->getInt())*60.0+float(tm_now.tm_min))/1440.0 *2.0*M_PI ),
                                                m_prp_lighting_position->getVector().z+m_prp_lighting_sun_distance->getFloat()*sin( (float(tm_now.tm_hour-m_prp_lighting_sun_rise_hour->getInt())*60.0+float(tm_now.tm_min))/1440.0 *2.0*M_PI ) ));
            m_tf2_local_wrt_lighting.setOrigin(vc3_local);
            m_qua_direction=getQuaternionFromDirection(-vc3_local);
        }
        else
        {
            m_tf2_local_wrt_lighting.setOrigin(tf2::Vector3(m_prp_lighting_position->getVector().x,m_prp_lighting_position->getVector().y,m_prp_lighting_position->getVector().z));
            m_qua_direction=getQuaternionFromDirection(tf2::Vector3(m_prp_lighting_direction->getVector().x,m_prp_lighting_direction->getVector().y,m_prp_lighting_direction->getVector().z));
        }

        // Derive orientation always from directional vector, the quaternion property gets updated only!
        m_prp_lighting_orientation->setQuaternion(Ogre::Quaternion(m_qua_direction.getW(),m_qua_direction.getX(),m_qua_direction.getY(),m_qua_direction.getZ()));
        m_tf2_local_wrt_lighting.setRotation(m_qua_direction);
        m_tf2_global=m_tf2_global_wrt_local*m_tf2_local_wrt_lighting;
        m_ogr_light_plugin->setPosition(Ogre::Vector3(
                                            m_tf2_global.getOrigin().getX(),
                                            m_tf2_global.getOrigin().getY(),
                                            m_tf2_global.getOrigin().getZ()));
        m_ogr_light_plugin->setDirection(getVector3FromQuaternion(Ogre::Quaternion(
                                            m_tf2_global.getRotation().getW(),
                                            m_tf2_global.getRotation().getX(),
                                            m_tf2_global.getRotation().getY(),
                                            m_tf2_global.getRotation().getZ())));
    }
    void RVizPluginDisplayLighting::UpdateLightingDetails()
    {
        m_ogr_light_plugin->setSpotlightInnerAngle(Ogre::Radian(m_prp_lighting_angle_inner->getFloat())); // Not supported via OpenGL!
        m_ogr_light_plugin->setSpotlightOuterAngle(Ogre::Radian(m_prp_lighting_angle_outer->getFloat()));
        m_ogr_light_plugin->setSpotlightFalloff(m_prp_lighting_falloff_rate->getFloat());
        m_ogr_light_plugin->setAttenuation(m_prp_lighting_range->getFloat(),
                                    m_prp_lighting_attenuation->getVector().x,
                                    m_prp_lighting_attenuation->getVector().y,
                                    m_prp_lighting_attenuation->getVector().z);
    }
    void RVizPluginDisplayLighting::InitializeLightingMarker()
    {
        // POINTLIGHT Marker
        m_msg_mrk_pointlight.header.frame_id="ias_wor_universe";
        m_msg_mrk_pointlight.header.stamp=m_hdl_node->get_clock()->now();
        //m_msg_mrk_spotlight.lifetime=rclcpp::Duration(0,100000000);
        m_msg_mrk_pointlight.ns=m_s_rviz_plugin_namespace+"mrk_lighting";
        m_msg_mrk_pointlight.id=0;
        m_msg_mrk_pointlight.type=visualization_msgs::msg::Marker::MESH_RESOURCE;
        m_msg_mrk_pointlight.action=visualization_msgs::msg::Marker::ADD;
        m_msg_mrk_pointlight.pose.position.x=0.0;
        m_msg_mrk_pointlight.pose.position.y=0.0;
        m_msg_mrk_pointlight.pose.position.z=0.0;
        m_msg_mrk_pointlight.pose.orientation.w=1.0;
        m_msg_mrk_pointlight.pose.orientation.x=0.0;
        m_msg_mrk_pointlight.pose.orientation.y=0.0;
        m_msg_mrk_pointlight.pose.orientation.z=0.0;
        m_msg_mrk_pointlight.scale.x=1.0;
        m_msg_mrk_pointlight.scale.y=1.0;
        m_msg_mrk_pointlight.scale.z=1.0;
        m_msg_mrk_pointlight.mesh_use_embedded_materials=true;
        m_msg_mrk_pointlight.mesh_resource="file://"+m_prp_lighting_marker_path->getStdString()+"pointlight/pointlight.dae";
        m_msg_mrk_pointlight.mesh_file.filename=m_prp_lighting_marker_path->getStdString()+"pointlight/pointlight.dae";

        // SPOTLIGHT Marker
        m_msg_mrk_spotlight.header.frame_id="ias_wor_universe";
        m_msg_mrk_spotlight.header.stamp=m_hdl_node->get_clock()->now();
        //m_msg_mrk_spotlight.lifetime=rclcpp::Duration(0,100000000);
        m_msg_mrk_spotlight.ns=m_s_rviz_plugin_namespace+"mrk_lighting";
        m_msg_mrk_spotlight.id=0;
        m_msg_mrk_spotlight.type=visualization_msgs::msg::Marker::MESH_RESOURCE;
        m_msg_mrk_spotlight.action=visualization_msgs::msg::Marker::ADD;
        m_msg_mrk_spotlight.pose.position.x=0.0;
        m_msg_mrk_spotlight.pose.position.y=0.0;
        m_msg_mrk_spotlight.pose.position.z=0.0;
        m_msg_mrk_spotlight.pose.orientation.w=1.0;
        m_msg_mrk_spotlight.pose.orientation.x=0.0;
        m_msg_mrk_spotlight.pose.orientation.y=0.0;
        m_msg_mrk_spotlight.pose.orientation.z=0.0;
        m_msg_mrk_spotlight.scale.x=1.0;
        m_msg_mrk_spotlight.scale.y=1.0;
        m_msg_mrk_spotlight.scale.z=1.0;
        m_msg_mrk_spotlight.mesh_use_embedded_materials=true;
        m_msg_mrk_spotlight.mesh_resource="file://"+m_prp_lighting_marker_path->getStdString()+"spotlight/spotlight.dae";
        m_msg_mrk_spotlight.mesh_file.filename=m_prp_lighting_marker_path->getStdString()+"spotlight/spotlight.dae";

        // SUNLIGHT Marker
        m_msg_mrk_sunlight.header.frame_id="ias_wor_universe";
        m_msg_mrk_sunlight.header.stamp=m_hdl_node->get_clock()->now();
        //m_msg_mrk_spotlight.lifetime=rclcpp::Duration(0,100000000);
        m_msg_mrk_sunlight.ns=m_s_rviz_plugin_namespace+"mrk_lighting";
        m_msg_mrk_sunlight.id=0;
        m_msg_mrk_sunlight.type=visualization_msgs::msg::Marker::MESH_RESOURCE;
        m_msg_mrk_sunlight.action=visualization_msgs::msg::Marker::ADD;
        m_msg_mrk_sunlight.pose.position.x=0.0;
        m_msg_mrk_sunlight.pose.position.y=0.0;
        m_msg_mrk_sunlight.pose.position.z=0.0;
        m_msg_mrk_sunlight.pose.orientation.w=1.0;
        m_msg_mrk_sunlight.pose.orientation.x=0.0;
        m_msg_mrk_sunlight.pose.orientation.y=0.0;
        m_msg_mrk_sunlight.pose.orientation.z=0.0;
        m_msg_mrk_sunlight.scale.x=0.00004;
        m_msg_mrk_sunlight.scale.y=0.00004;
        m_msg_mrk_sunlight.scale.z=0.00004;
        m_msg_mrk_sunlight.mesh_use_embedded_materials=true;
        m_msg_mrk_sunlight.mesh_resource="file://"+m_prp_lighting_marker_path->getStdString()+"sunlight/sunlight.dae";
        m_msg_mrk_sunlight.mesh_file.filename=m_prp_lighting_marker_path->getStdString()+"sunlight/sunlight.dae";
    }
    void RVizPluginDisplayLighting::UpdateLightingMarker()
    {
        m_msg_mrk_pointlight.header.frame_id=m_prp_lighting_tf_frame->getFrameStd();
        m_msg_mrk_pointlight.header.stamp=m_hdl_node->get_clock()->now();
        m_msg_mrk_spotlight.header.frame_id=m_prp_lighting_tf_frame->getFrameStd();
        m_msg_mrk_spotlight.header.stamp=m_hdl_node->get_clock()->now();
        m_msg_mrk_sunlight.header.frame_id=m_prp_lighting_tf_frame->getFrameStd();
        m_msg_mrk_sunlight.header.stamp=m_hdl_node->get_clock()->now();
        m_pos_mrk_pose.position.x=m_tf2_local_wrt_lighting.getOrigin().getX();
        m_pos_mrk_pose.position.y=m_tf2_local_wrt_lighting.getOrigin().getY();
        m_pos_mrk_pose.position.z=m_tf2_local_wrt_lighting.getOrigin().getZ();
        m_pos_mrk_pose.orientation.w=m_tf2_local_wrt_lighting.getRotation().getW();
        m_pos_mrk_pose.orientation.x=m_tf2_local_wrt_lighting.getRotation().getX();
        m_pos_mrk_pose.orientation.y=m_tf2_local_wrt_lighting.getRotation().getY();
        m_pos_mrk_pose.orientation.z=m_tf2_local_wrt_lighting.getRotation().getZ();
        m_msg_mrk_pointlight.pose=m_pos_mrk_pose;
        m_msg_mrk_spotlight.pose=m_pos_mrk_pose;
        m_msg_mrk_sunlight.pose=m_pos_mrk_pose;
        if(m_prp_lighting_type->getOptionInt()==Ogre::Light::LT_POINT)
        {
            m_msg_mrk_pointlight.action=visualization_msgs::msg::Marker::ADD;
            m_pub_mrk_light->publish(m_msg_mrk_pointlight);
        }
        else if(m_prp_lighting_type->getOptionInt()==Ogre::Light::LT_SPOTLIGHT)
        {
            m_msg_mrk_spotlight.action=visualization_msgs::msg::Marker::ADD;
            m_pub_mrk_light->publish(m_msg_mrk_spotlight);
        }
        else if(m_prp_lighting_type->getOptionInt()==4)
        {
            m_msg_mrk_sunlight.action=visualization_msgs::msg::Marker::ADD;
            m_pub_mrk_light->publish(m_msg_mrk_sunlight);
        }
        else
        {
            DisableLightingMarker();
        }
    }
    void RVizPluginDisplayLighting::DisableLightingMarker()
    {
        m_msg_mrk_pointlight.action=visualization_msgs::msg::Marker::DELETEALL;
        m_msg_mrk_spotlight.action=visualization_msgs::msg::Marker::DELETEALL;
        m_msg_mrk_sunlight.action=visualization_msgs::msg::Marker::DELETEALL;
        m_pub_mrk_light->publish(m_msg_mrk_pointlight);
    }
    tf2::Quaternion RVizPluginDisplayLighting::getQuaternionFromDirection(tf2::Vector3 vc3_input)
    {
        // Avoid "gimbal lock" with up vector, limit to +/-89deg for marker visualization only!
        if(vc3_input.getX()==0.0 && vc3_input.getY()==0.0 && vc3_input.getZ()==1.0)
        {
            vc3_input.setX(0.017452406);
            vc3_input.setY(0.0);
            vc3_input.setZ(0.999847695);
        }
        else if(vc3_input.getX()==0.0 && vc3_input.getY()==0.0 && vc3_input.getZ()==-1.0)
        {
            vc3_input.setX(0.017452406);
            vc3_input.setY(0.0);
            vc3_input.setZ(-0.999847695);
        }
        else
        {
            // Do nothing for now...
        }
        tf2::Vector3 vc3_forward=vc3_input.normalized();
        tf2::Vector3 vc3_up(0.0,0.0,1.0);
        tf2::Vector3 vc3_right = vc3_up.cross(vc3_forward);
        vc3_right.normalize();
        vc3_up=vc3_forward.cross(vc3_right);
        vc3_up.normalize();
        tf2::Matrix3x3 mat_rotation(vc3_forward.x(),vc3_right.x(),vc3_up.x(),
                                    vc3_forward.y(),vc3_right.y(),vc3_up.y(),
                                    vc3_forward.z(),vc3_right.z(),vc3_up.z());
        tf2::Quaternion qua_output;
        mat_rotation.getRotation(qua_output);
        return qua_output;
    }
    Ogre::Vector3 RVizPluginDisplayLighting::getVector3FromQuaternion(Ogre::Quaternion oqu_input)
    {
        tf2::Vector3 vc3_forward(1.0,0.0,0.0);
        tf2::Vector3 vc3_output=tf2::quatRotate(tf2::Quaternion(oqu_input.x,oqu_input.y,oqu_input.z,oqu_input.w),vc3_forward);
        return Ogre::Vector3(vc3_output.getX(),vc3_output.getY(),vc3_output.getZ());
    }

    // Initialize static properties
    int RVizPluginDisplayLighting::si_resource_name_count;

    // Default constructor and destructor
    RVizPluginDisplayLighting::RVizPluginDisplayLighting()
    {
        // Do nothing for now...
    }
    RVizPluginDisplayLighting::~RVizPluginDisplayLighting()
    {
        // Do nothing for now...
    }

}  // namespace rviz_plugin_display_lighting

#include<pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_display_lighting::RVizPluginDisplayLighting,rviz_common::Display)
