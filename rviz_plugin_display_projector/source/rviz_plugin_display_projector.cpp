#include<rviz_plugin_display_projector.hpp>



namespace rviz_plugin_display_projector
{
    // Inherited methods
    void RVizPluginDisplayProjector::onInitialize()
    {
        // Initialize ROS2 properties
        m_hdl_node_rviz=context_->getRosNodeAbstraction().lock();
        m_hdl_node=m_hdl_node_rviz->get_raw_node();
        m_hdl_it=std::make_shared<image_transport::ImageTransport>(m_hdl_node);

        // Initialize helper properties
        si_resource_name_count++;
        m_s_rviz_plugin_namespace="rviz_plugin_display_projector_"+std::to_string(si_resource_name_count)+"/";
        std::stringstream sst_topic;
        sst_topic << m_s_rviz_plugin_namespace << "image_raw";

        // Initialize RViz properties
        QRegExp qre_projector_topic_filter("image"); qre_projector_topic_filter.setCaseSensitivity(Qt::CaseInsensitive);
        m_prp_projector_image_topic=std::make_unique<rviz_common::properties::RosFilteredTopicProperty>("Image Topic",sst_topic.str().c_str(),"sensor_msgs/msg/Image","Image Topic.",qre_projector_topic_filter,this,SLOT(cb_prp_update_projector_topics()));
        m_prp_projector_image_topic->initialize(m_hdl_node_rviz);
        m_prp_projector_transport=std::make_unique<rviz_common::properties::EnumProperty>("Image Transport","raw","Image Transport.",this,SLOT(cb_prp_update_projector_transports()));
        m_prp_projector_transport->addOption("raw");
        m_prp_projector_transport->addOption("compressed");
        m_prp_projector_transport->addOption("theora");
        m_i_projector_res_x=320; m_i_projector_res_y=240;
        m_prp_projector_res_x=std::make_unique<rviz_common::properties::IntProperty>("Resolution X",m_i_projector_res_x,"Resolution X.",this,SLOT(cb_prp_update_projector_res()));
        m_prp_projector_res_y=std::make_unique<rviz_common::properties::IntProperty>("Resolution Y",m_i_projector_res_y,"Resolution Y.",this,SLOT(cb_prp_update_projector_res()));
        m_prp_projector_res_x->setReadOnly(true);
        m_prp_projector_res_y->setReadOnly(true);
        m_prp_projector_width=std::make_unique<rviz_common::properties::FloatProperty>("Width",1.0,"Width.",this,SLOT(cb_prp_update_projector_width()));
        m_prp_projector_height=std::make_unique<rviz_common::properties::FloatProperty>("Height",1.0,"Height.",this,SLOT(cb_prp_update_projector_height()));
        m_prp_projector_tf_frame=std::make_unique<rviz_common::properties::TfFrameProperty>("TF Frame",rviz_common::properties::TfFrameProperty::FIXED_FRAME_STRING,"TF Frame.",this,(rviz_common::FrameManager*)context_->getFrameManager(),true,SLOT(cb_prp_update_projector_tf_frame()));
        m_prp_projector_position=std::make_unique<rviz_common::properties::VectorProperty>("Position",Ogre::Vector3::ZERO,"Position.",this,SLOT(cb_prp_update_projector_position()));
        m_prp_projector_orientation=std::make_unique<rviz_common::properties::QuaternionProperty>("Orientation",Ogre::Quaternion::IDENTITY,"Orientation.",this,SLOT(cb_prp_update_projector_orientation()));
        m_prp_projector_position->setReadOnly(true);
        m_prp_projector_orientation->setReadOnly(true);
        m_prp_projector_tf_refresh=std::make_unique<rviz_common::properties::BoolProperty>("Refresh TF Frame",false,"Refresh TF Frame.",this,SLOT(cb_prp_update_projector_tf_refresh()));
        m_prp_projector_border_thickness=std::make_unique<rviz_common::properties::FloatProperty>("Border Thickness",0.025,"Border Thickness.",this,SLOT(cb_prp_update_projector_border_thickness()));
        m_prp_projector_border_thickness->setMin(0.0);
        m_prp_projector_border_thickness->setMax(1.0);
        m_prp_projector_border_color=std::make_unique<rviz_common::properties::ColorProperty>("Border Color",QColor(245,194,17),"Border Color.",this,SLOT(cb_prp_update_projector_border_color()));
        m_prp_projector_alpha=std::make_unique<rviz_common::properties::FloatProperty>("Alpha",1.0,"Alpha.",this,SLOT(cb_prp_update_projector_alpha()));
        m_prp_projector_alpha->setMin(0.1);
        m_prp_projector_alpha->setMax(1.0);

        // Initialize OGRE properties
        UpdateProjectorOgreResources();

        // Initialize projector border
        m_shp_projector_border_left=std::make_unique<rviz_rendering::Shape>(rviz_rendering::Shape::Type::Cylinder,scene_manager_,scene_node_);
        m_shp_projector_border_right=std::make_unique<rviz_rendering::Shape>(rviz_rendering::Shape::Type::Cylinder,scene_manager_,scene_node_);
        m_shp_projector_border_top=std::make_unique<rviz_rendering::Shape>(rviz_rendering::Shape::Type::Cylinder,scene_manager_,scene_node_);
        m_shp_projector_border_bottom=std::make_unique<rviz_rendering::Shape>(rviz_rendering::Shape::Type::Cylinder,scene_manager_,scene_node_);
        UpdateProjectorTFFrame();

        // Initialize ROS2 subscribers and publishers
        UpdateProjectorTopicsAndTransports();

        setStatusStd(rviz_common::properties::StatusProperty::Ok,"Projector","Ok!");
        RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_display_projector::RVizPluginDisplayProjector - Initialized!");
    }
    void RVizPluginDisplayProjector::onDisable()
    {
        // Set Alpha to 0.0
        m_map_material->setDepthWriteEnabled(false);
        m_tus_texture_unit_state=m_pas_texture->getTextureUnitState(0);
        m_tus_texture_unit_state->setAlphaOperation(
            Ogre::LBX_BLEND_MANUAL,
            Ogre::LBS_MANUAL,
            Ogre::LBS_MANUAL,
            0.0,
            0.0,
            0.0);
        m_tus_texture_unit_state->setColourOperation(Ogre::LBO_REPLACE);
    }
    void RVizPluginDisplayProjector::onEnable()
    {
        // Set Alpha to 1.0
        m_map_material->setDepthWriteEnabled(true);
        m_tus_texture_unit_state=m_pas_texture->getTextureUnitState(0);
        m_tus_texture_unit_state->setAlphaOperation(
            Ogre::LBX_BLEND_MANUAL,
            Ogre::LBS_MANUAL,
            Ogre::LBS_MANUAL,
            1.0,
            1.0,
            1.0);
        m_tus_texture_unit_state->setColourOperation(Ogre::LBO_REPLACE);
    }
    void RVizPluginDisplayProjector::reset()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayProjector::update(float wall_dt,float ros_dt)
    {
        if(m_prp_projector_tf_refresh->getBool()==true)
        {
            UpdateProjectorTFFrame(); // If the frame is moving, update the projected texture accordingly
        }
    }

    // Callbacks to process ROS2 messages
    void RVizPluginDisplayProjector::cb_sub_img_projector(const sensor_msgs::msg::Image::ConstSharedPtr& msg_img_projector)
    {
        // Convert ROS2 Image Message to OpenCV Image
        try
        {
            cv_bridge::CvImagePtr cv_img_ptr;
            cv_img_ptr=cv_bridge::toCvCopy(msg_img_projector,sensor_msgs::image_encodings::RGBA8);
            m_mat_img_projector=cv_img_ptr->image;

            if(m_mat_img_projector.cols==m_i_projector_res_x && m_mat_img_projector.rows==m_i_projector_res_y)
            {
                // Do nothing for now...
            }
            else
            {
                // Update actual image resolution properties
                m_i_projector_res_x=m_mat_img_projector.cols;
                m_i_projector_res_y=m_mat_img_projector.rows;
                m_prp_projector_res_x->setInt(m_i_projector_res_x);
                m_prp_projector_res_y->setInt(m_i_projector_res_y);
                UpdateProjectorOgreTextureResolution(m_i_projector_res_x,m_i_projector_res_y);
                UpdateProjectorPose();
            }

            UpdateProjectorOgreTexture();
        }
        catch(cv_bridge::Exception& e)
        {
            RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_display_projector::RVizPluginDisplayProjector - cv_bridge::Exception: " << e.what());
        }
    }

    // Callbacks to process property updates
    void RVizPluginDisplayProjector::cb_prp_update_projector_topics()
    {
        UpdateProjectorTopicsAndTransports();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_transports()
    {
        UpdateProjectorTopicsAndTransports();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_res()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_width()
    {
        m_hdl_node_scene->setScale(m_prp_projector_width->getFloat(),m_prp_projector_height->getFloat(),1.0);
        UpdateProjectorOgreTextureResolution(m_i_projector_res_x,m_i_projector_res_y);
        UpdateProjectorPose();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_height()
    {
        m_hdl_node_scene->setScale(m_prp_projector_width->getFloat(),m_prp_projector_height->getFloat(),1.0);
        UpdateProjectorOgreTextureResolution(m_i_projector_res_x,m_i_projector_res_y);
        UpdateProjectorPose();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_tf_frame()
    {
        UpdateProjectorTFFrame();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_position()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_orientation()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_tf_refresh()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_border_thickness()
    {
        UpdateProjectorPose();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_border_color()
    {
        UpdateProjectorPose();
    }
    void RVizPluginDisplayProjector::cb_prp_update_projector_alpha()
    {
        UpdateProjectorAlpha();
    }

    // Helper methods
    void RVizPluginDisplayProjector::UpdateProjectorOgreResources()
    {
        // Create texture
        m_tep_texture=Ogre::TextureManager::getSingleton().createManual("ProjectorTexture"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_2D,m_prp_projector_res_x->getInt(),m_prp_projector_res_y->getInt(),0,Ogre::PF_BYTE_RGBA,Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE); // TU_DYNAMIC

        // Create proper material that has culling disabled (image is visible from both sides!)
        m_map_material=Ogre::MaterialManager::getSingleton().create("ProjectorMaterial"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        m_map_material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);

        // Create TexturePass and TextureUnitState
        //m_map_material->setLightingEnabled(false);
        m_pas_texture=m_map_material->getTechnique(0)->getPass(0);
        m_pas_texture->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        m_tus_texture_unit_state=m_pas_texture->createTextureUnitState("ProjectorTexture"+std::to_string(si_resource_name_count));
        UpdateProjectorAlpha();

        // Create Plane to render the image to
        m_pla_plane.redefine(Ogre::Vector3::UNIT_Z,Ogre::Vector3(0,0,0));
        Ogre::MeshManager::getSingleton().createPlane("ProjectorPlane"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,m_pla_plane,m_prp_projector_width->getFloat(),m_prp_projector_height->getFloat(),1,1,true,1,1.0f,1.0f,Ogre::Vector3::UNIT_Y);

        // Create Entity and attach material
        m_ent_entity=scene_manager_->createEntity("ProjectorPlane"+std::to_string(si_resource_name_count));
        m_ent_entity->setMaterialName("ProjectorMaterial"+std::to_string(si_resource_name_count));

        // Attach Entity to Scene Node
        m_hdl_node_scene=scene_manager_->getRootSceneNode()->createChildSceneNode();
        m_hdl_node_scene->setPosition(Ogre::Vector3(0.0,0.0,0.0));
        m_hdl_node_scene->setOrientation(Ogre::Quaternion(1.0,0.0,0.0,0.0));
        m_hdl_node_scene->attachObject(m_ent_entity);
    }
    void RVizPluginDisplayProjector::UpdateProjectorOgreTexture()
    {
        // Simply Copy Texture Contents To m_hpb_buffer
        m_hpb_buffer=m_tep_texture->getBuffer();
        m_hpb_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        //const Ogre::PixelBox& pixelBox=m_hpb_buffer->getCurrentLock().data;
        m_ui8_buffer=static_cast<uint8_t*>(m_hpb_buffer->getCurrentLock().data);
        memcpy(m_ui8_buffer,m_mat_img_projector.data,m_mat_img_projector.total()*m_mat_img_projector.elemSize());
        m_hpb_buffer->unlock();
    }
    void RVizPluginDisplayProjector::UpdateProjectorOgreTextureResolution(int i_res_x,int i_res_y)
    {
        // Detach entity
        m_hdl_node_scene->detachObject(m_ent_entity);

        // Create texture
        Ogre::TextureManager::getSingleton().remove("ProjectorTexture"+std::to_string(si_resource_name_count));
        m_tep_texture=Ogre::TextureManager::getSingleton().createManual("ProjectorTexture"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_2D,i_res_x,i_res_y,0,Ogre::PF_BYTE_RGBA,Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

        // Create proper material that has culling disabled (image is visible from both sides!)
        Ogre::MaterialManager::getSingleton().remove("ProjectorMaterial"+std::to_string(si_resource_name_count));
        m_map_material=Ogre::MaterialManager::getSingleton().create("ProjectorMaterial"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        m_map_material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);

        // Create TexturePass and TextureUnitState
        //m_map_material->setLightingEnabled(false);
        m_pas_texture->removeAllTextureUnitStates();
        m_pas_texture=m_map_material->getTechnique(0)->getPass(0);
        m_pas_texture->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        m_tus_texture_unit_state=m_pas_texture->createTextureUnitState("ProjectorTexture"+std::to_string(si_resource_name_count));
        UpdateProjectorAlpha();

        // Create Plane to render the image to
        Ogre::MeshManager::getSingleton().remove("ProjectorPlane"+std::to_string(si_resource_name_count));
        m_pla_plane.redefine(Ogre::Vector3::UNIT_Z,Ogre::Vector3(0,0,0));
        Ogre::MeshManager::getSingleton().createPlane("ProjectorPlane"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,m_pla_plane,m_prp_projector_width->getFloat(),m_prp_projector_height->getFloat(),1,1,true,1,1.0f,1.0f,Ogre::Vector3::UNIT_Y);

        // Create Entity and attach Material
        scene_manager_->destroyEntity("ProjectorPlane"+std::to_string(si_resource_name_count));
        m_ent_entity=scene_manager_->createEntity("ProjectorPlane"+std::to_string(si_resource_name_count));
        m_ent_entity->setMaterialName("ProjectorMaterial"+std::to_string(si_resource_name_count));

        // Re-attach Entity to SceneNode
        m_hdl_node_scene=scene_manager_->getRootSceneNode()->createChildSceneNode();
        m_hdl_node_scene->setPosition(Ogre::Vector3(0.0,0.0,0.0));
        m_hdl_node_scene->setOrientation(Ogre::Quaternion(1.0,0.0,0.0,0.0));
        m_hdl_node_scene->attachObject(m_ent_entity);
    }
    void RVizPluginDisplayProjector::UpdateProjectorTopicsAndTransports()
    {
        m_sub_img_projector.shutdown();
        std::string s_image_transport=m_prp_projector_transport->getStdString();
        image_transport::TransportHints ith_transport_hints(m_hdl_node.get(),s_image_transport,"image_transport");
        m_sub_img_projector=image_transport::create_subscription(m_hdl_node.get(),
                                                         m_prp_projector_image_topic->getTopicStd(),
                                                         std::bind(&RVizPluginDisplayProjector::cb_sub_img_projector,this,std::placeholders::_1),
                                                         ith_transport_hints.getTransport(),
                                                         rmw_qos_profile_default);
    }
    void RVizPluginDisplayProjector::UpdateProjectorTFFrame()
    {
        if(context_->getFrameManager()->getTransform(m_prp_projector_tf_frame->getFrameStd(),m_ov3_position,m_oqu_orientation))
        {
            m_tf2_projector.setOrigin(tf2::Vector3(m_ov3_position.x,m_ov3_position.y,m_ov3_position.z));
            m_tf2_projector.setRotation(tf2::Quaternion(m_oqu_orientation.x,m_oqu_orientation.y,m_oqu_orientation.z,m_oqu_orientation.w));
            UpdateProjectorPose();
            setStatusStd(rviz_common::properties::StatusProperty::Ok,"Projector","Ok!");
        }
        else
        {
            setStatusStd(rviz_common::properties::StatusProperty::Warn,"Projector","No valid transform!");
        }

    }
    void RVizPluginDisplayProjector::UpdateProjectorPose()
    {
        // Update pose of rendered image
        m_hdl_node_scene->setPosition(m_ov3_position);
        m_hdl_node_scene->setOrientation(m_oqu_orientation);
        m_prp_projector_position->setVector(m_ov3_position);
        m_prp_projector_orientation->setQuaternion(m_oqu_orientation);
        UpdateProjectorBorder();
    }
    void RVizPluginDisplayProjector::UpdateProjectorBorder()
    {
        m_shp_projector_border_left->setScale(Ogre::Vector3(m_prp_projector_border_thickness->getFloat(),m_prp_projector_height->getFloat(),m_prp_projector_border_thickness->getFloat()));
        m_shp_projector_border_right->setScale(Ogre::Vector3(m_prp_projector_border_thickness->getFloat(),m_prp_projector_height->getFloat(),m_prp_projector_border_thickness->getFloat()));
        m_shp_projector_border_top->setScale(Ogre::Vector3(m_prp_projector_width->getFloat(),m_prp_projector_border_thickness->getFloat(),m_prp_projector_border_thickness->getFloat()));
        m_shp_projector_border_bottom->setScale(Ogre::Vector3(m_prp_projector_width->getFloat(),m_prp_projector_border_thickness->getFloat(),m_prp_projector_border_thickness->getFloat()));

        tf2::Vector3 vc3_left(m_prp_projector_width->getFloat()/2.0,0.0,0.0); vc3_left=m_tf2_projector*vc3_left;
        tf2::Vector3 vc3_right(-m_prp_projector_width->getFloat()/2.0,0.0,0.0); vc3_right=m_tf2_projector*vc3_right;
        tf2::Vector3 vc3_top(0.0,m_prp_projector_height->getFloat()/2.0,0.0); vc3_top=m_tf2_projector*vc3_top;
        tf2::Vector3 vc3_bottom(0.0,-m_prp_projector_height->getFloat()/2.0,0.0); vc3_bottom=m_tf2_projector*vc3_bottom;
        m_shp_projector_border_left->setPosition(Ogre::Vector3(vc3_left.getX(),vc3_left.getY(),vc3_left.getZ()));
        m_shp_projector_border_right->setPosition(Ogre::Vector3(vc3_right.getX(),vc3_right.getY(),vc3_right.getZ()));
        m_shp_projector_border_top->setPosition(Ogre::Vector3(vc3_top.getX(),vc3_top.getY(),vc3_top.getZ()));
        m_shp_projector_border_bottom->setPosition(Ogre::Vector3(vc3_bottom.getX(),vc3_bottom.getY(),vc3_bottom.getZ()));
        m_shp_projector_border_left->setOrientation(m_oqu_orientation);
        m_shp_projector_border_right->setOrientation(m_oqu_orientation);
        m_shp_projector_border_top->setOrientation(m_oqu_orientation);
        m_shp_projector_border_bottom->setOrientation(m_oqu_orientation);

        Ogre::ColourValue cov_color(m_prp_projector_border_color->getOgreColor().r,
                                    m_prp_projector_border_color->getOgreColor().g,
                                    m_prp_projector_border_color->getOgreColor().b,
                                    m_prp_projector_border_color->getOgreColor().a);
        m_shp_projector_border_left->setColor(cov_color);
        m_shp_projector_border_right->setColor(cov_color);
        m_shp_projector_border_top->setColor(cov_color);
        m_shp_projector_border_bottom->setColor(cov_color);
    }
    void RVizPluginDisplayProjector::UpdateProjectorAlpha()
    {
        if(m_prp_projector_alpha->getFloat()==1.0)
        {
            // Actually make texture intransparent no matter what bg
            m_map_material->setDepthWriteEnabled(true);
            m_tus_texture_unit_state=m_pas_texture->getTextureUnitState(0);
        }
        else
        {
            m_map_material->setDepthWriteEnabled(false);
            m_tus_texture_unit_state=m_pas_texture->getTextureUnitState(0);
        }
        m_tus_texture_unit_state->setAlphaOperation(
            Ogre::LBX_BLEND_MANUAL,
            Ogre::LBS_MANUAL,
            Ogre::LBS_MANUAL,
            m_prp_projector_alpha->getFloat(),
            m_prp_projector_alpha->getFloat(),
            m_prp_projector_alpha->getFloat());
        m_tus_texture_unit_state->setColourOperation(Ogre::LBO_REPLACE);
    }

    // Initialize static properties
    int RVizPluginDisplayProjector::si_resource_name_count;

    // Default constructor and destructor
    RVizPluginDisplayProjector::RVizPluginDisplayProjector()
    {
        // Do nothing for now...
    }
    RVizPluginDisplayProjector::~RVizPluginDisplayProjector()
    {
        // Do nothing for now...
    }

}  // namespace rviz_plugin_display_projector

#include<pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_display_projector::RVizPluginDisplayProjector,rviz_common::Display)
