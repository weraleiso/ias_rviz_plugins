#include<rviz_plugin_display_stereoscopic.hpp>



namespace rviz_plugin_display_stereoscopic
{
    // Inherited methods
    void RVizPluginDisplayStereoscopic::onInitialize()
    {
        // Initialize ROS2 properties
        m_hdl_node_rviz=context_->getRosNodeAbstraction().lock();
        m_hdl_node=m_hdl_node_rviz->get_raw_node();
        m_hdl_it=std::make_shared<image_transport::ImageTransport>(m_hdl_node);

        // Initialize helper properties
        si_resource_name_count++;
        m_s_rviz_plugin_namespace="rviz_plugin_display_stereoscopic_"+std::to_string(si_resource_name_count)+"/";
        m_i_stereoscopic_view_res_x=640;
        m_i_stereoscopic_view_res_y=720;
        m_b_ogr_camera_initialized=false;

        // Initialize RViz properties
        m_prp_stereoscopic_view_image_topic=std::make_unique<rviz_common::properties::RosTopicProperty>("Image Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"view"),"sensor_msgs/msg/Image","Image Topic.",this,SLOT(cb_prp_update_stereoscopic_topics()));
        m_prp_stereoscopic_view_image_topic->initialize(m_hdl_node_rviz);
        m_prp_stereoscopic_view_res_x=std::make_unique<rviz_common::properties::IntProperty>("Per Eye Resolution X",m_i_stereoscopic_view_res_x,"Per Eye Resolution X.",this,SLOT(cb_prp_update_stereoscopic_resolution()));
        m_prp_stereoscopic_view_res_y=std::make_unique<rviz_common::properties::IntProperty>("Per Eye Resolution Y",m_i_stereoscopic_view_res_y,"Per Eye Resolution Y.",this,SLOT(cb_prp_update_stereoscopic_resolution()));
        m_prp_stereoscopic_ipd=std::make_unique<rviz_common::properties::FloatProperty>("Interpupillary Distance",0.065,"Interpupillary Distance.",this,SLOT(cb_prp_update_stereoscopic_ipd()));
        m_prp_stereoscopic_dist=std::make_unique<rviz_common::properties::FloatProperty>("Radial Distortion",1.0,"Radial Distortion.",this,SLOT(cb_prp_update_stereoscopic_distortion()));

        // Initialize stereoscopic view
        UpdateStereoscopicTopicsAndTransports();

        // Init timers
        tmr_init_delayed=m_hdl_node->create_wall_timer(std::chrono::milliseconds(3000),std::bind(&RVizPluginDisplayStereoscopic::cb_tmr_init_delayed,this));

        RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_display_stereoscopic::RVizPluginDisplayStereoscopic - Initialized!");
    }
    void RVizPluginDisplayStereoscopic::onDisable()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayStereoscopic::onEnable()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayStereoscopic::reset()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayStereoscopic::update(float wall_dt,float ros_dt)
    {
        if(m_b_ogr_camera_initialized)
        {
            UpdateStereoscopicExtrinsics();
            UpdateStereoscopicIntrinsics();
            UpdateStereoscopicView();
        }
    }

    // Callbacks to process ROS2 messages
    void RVizPluginDisplayStereoscopic::cb_tmr_init_delayed()
    {
        InitStereoscopicTexture();
        UpdateStereoscopicExtrinsics();
        UpdateStereoscopicIntrinsics();
        tmr_init_delayed->cancel();
        m_b_ogr_camera_initialized=true;
    }

    // Callbacks to process property updates
    void RVizPluginDisplayStereoscopic::cb_prp_update_stereoscopic_topics()
    {
        UpdateStereoscopicTopicsAndTransports();
    }
    void RVizPluginDisplayStereoscopic::cb_prp_update_stereoscopic_resolution()
    {
        UpdateStereoscopicResultion();
    }
    void RVizPluginDisplayStereoscopic::cb_prp_update_stereoscopic_ipd()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayStereoscopic::cb_prp_update_stereoscopic_distortion()
    {
        // Do nothing for now...
    }

    // Helper methods
    void RVizPluginDisplayStereoscopic::CleanupStereoscopicTexture()
    {
        m_ogr_render_texture_eye_left->setAutoUpdated(false);
        m_ogr_render_texture_eye_left->setActive(false);
        Ogre::TextureManager::getSingleton().remove(m_ogr_texture_ptr_eye_left->getName());
        context_->getSceneManager()->destroyCamera("display_stereoscopic_camera_eye_left"+std::to_string(si_resource_name_count));
        m_ogr_texture_ptr_eye_left->freeInternalResources();

        m_ogr_render_texture_eye_right->setAutoUpdated(false);
        m_ogr_render_texture_eye_right->setActive(false);
        Ogre::TextureManager::getSingleton().remove(m_ogr_texture_ptr_eye_right->getName());
        context_->getSceneManager()->destroyCamera("display_stereoscopic_camera_eye_right"+std::to_string(si_resource_name_count));
        m_ogr_texture_ptr_eye_right->freeInternalResources();
    }
    void RVizPluginDisplayStereoscopic::InitStereoscopicTexture()
    {
        // Init OGRE camera
        Ogre::SceneManager::CameraIterator ogr_camera_itr=context_->getSceneManager()->getCameraIterator();
        while(ogr_camera_itr.hasMoreElements())
        {
            Ogre::String ogr_s_camera_name=ogr_camera_itr.peekNextKey();
            Ogre::Camera* ogr_camera=ogr_camera_itr.getNext();
            if(ogr_camera->getViewport()!=nullptr && ogr_camera->isAttached())
            {
                m_ogr_camera_current=ogr_camera;
                //RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_display_stereoscopic::RVizPluginDisplayStereoscopic - Selected Camera: " << ogr_s_camera_name);
            }
        }

        // Init OGRE textures with viewports
        m_ogr_camera_eye_left=context_->getSceneManager()->createCamera("display_stereoscopic_camera_eye_left"+std::to_string(si_resource_name_count));
        m_ogr_camera_eye_left->setNearClipDistance(m_ogr_camera_current->getNearClipDistance());
        m_ogr_texture_ptr_eye_left=Ogre::TextureManager::getSingleton().createManual(
                                        "display_stereoscopic_texture_eye_left"+std::to_string(si_resource_name_count),
                                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                        Ogre::TEX_TYPE_2D,
                                        m_i_stereoscopic_view_res_x,
                                        m_i_stereoscopic_view_res_y,
                                        0,
                                        Ogre::PF_R8G8B8,
                                        Ogre::TU_RENDERTARGET);
        m_ogr_render_texture_eye_left=m_ogr_texture_ptr_eye_left->getBuffer()->getRenderTarget();
        m_ogr_render_texture_eye_left->addViewport(m_ogr_camera_eye_left);
        m_ogr_render_texture_eye_left->getViewport(0)->setClearEveryFrame(true);
        m_ogr_render_texture_eye_left->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
        m_ogr_render_texture_eye_left->getViewport(0)->setOverlaysEnabled(false);

        m_ogr_camera_eye_right=context_->getSceneManager()->createCamera("display_stereoscopic_camera_eye_right"+std::to_string(si_resource_name_count));
        m_ogr_camera_eye_right->setNearClipDistance(m_ogr_camera_eye_left->getNearClipDistance());
        m_ogr_texture_ptr_eye_right=Ogre::TextureManager::getSingleton().createManual(
                                        "display_stereoscopic_texture_eye_right"+std::to_string(si_resource_name_count),
                                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                        Ogre::TEX_TYPE_2D,
                                        m_i_stereoscopic_view_res_x,
                                        m_i_stereoscopic_view_res_y,
                                        0,
                                        Ogre::PF_R8G8B8,
                                        Ogre::TU_RENDERTARGET);
        m_ogr_render_texture_eye_right=m_ogr_texture_ptr_eye_right->getBuffer()->getRenderTarget();
        m_ogr_render_texture_eye_right->addViewport(m_ogr_camera_eye_right);
        m_ogr_render_texture_eye_right->getViewport(0)->setClearEveryFrame(true);
        m_ogr_render_texture_eye_right->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
        m_ogr_render_texture_eye_right->getViewport(0)->setOverlaysEnabled(false);

        // Init OpenCV textures
        mat_img_stereoscopic_eye_left=new cv::Mat(m_i_stereoscopic_view_res_y,m_i_stereoscopic_view_res_x,CV_8UC3);
        mat_img_stereoscopic_eye_right=new cv::Mat(m_i_stereoscopic_view_res_y,m_i_stereoscopic_view_res_x,CV_8UC3);
        mat_img_stereoscopic_eye_left_dist=new cv::Mat(m_i_stereoscopic_view_res_y,m_i_stereoscopic_view_res_x,CV_8UC3);
        mat_img_stereoscopic_eye_right_dist=new cv::Mat(m_i_stereoscopic_view_res_y,m_i_stereoscopic_view_res_x,CV_8UC3);
        mat_img_stereoscopic_portable=new cv::Mat(m_i_stereoscopic_view_res_y,m_i_stereoscopic_view_res_x*2,CV_8UC3);
    }
    void RVizPluginDisplayStereoscopic::UpdateStereoscopicTopicsAndTransports()
    {
        if(m_pub_img_stereoscopic_portable) m_pub_img_stereoscopic_portable.shutdown();
        m_pub_img_stereoscopic_portable=m_hdl_it->advertise(m_prp_stereoscopic_view_image_topic->getTopicStd(),1);
    }
    void RVizPluginDisplayStereoscopic::UpdateStereoscopicResultion()
    {
        m_i_stereoscopic_view_res_x=m_prp_stereoscopic_view_res_x->getInt();
        m_i_stereoscopic_view_res_y=m_prp_stereoscopic_view_res_y->getInt();
        CleanupStereoscopicTexture();
        InitStereoscopicTexture();
    }
    void RVizPluginDisplayStereoscopic::UpdateStereoscopicExtrinsics()
    {
        m_ov3_camera_position=Ogre::Vector3(m_ogr_camera_current->getPositionForViewUpdate());
        Ogre::Vector3 ogr_vce3_camera_ipd(m_prp_stereoscopic_ipd->getFloat(),0.0,0.0);
        m_oqu_camera_orientation=m_ogr_camera_current->getOrientationForViewUpdate();
        m_ogr_camera_eye_left->setPosition(m_ov3_camera_position);
        m_ogr_camera_eye_left->setOrientation(m_oqu_camera_orientation);
        m_ov3_camera_position=m_ov3_camera_position+(m_oqu_camera_orientation*ogr_vce3_camera_ipd);
        m_ogr_camera_eye_right->setPosition(m_ov3_camera_position);
        m_ogr_camera_eye_right->setOrientation(m_oqu_camera_orientation);
    }
    void RVizPluginDisplayStereoscopic::UpdateStereoscopicIntrinsics()
    {
        m_mat_camera_matrix=cv::Mat(3,3,cv::DataType<double>::type);
        m_mat_camera_matrix.at<double>(0,0)=m_i_stereoscopic_view_res_x/2.0;
        m_mat_camera_matrix.at<double>(0,1)=0;
        m_mat_camera_matrix.at<double>(0,2)=m_i_stereoscopic_view_res_x/2.0;
        m_mat_camera_matrix.at<double>(1,0)=0;
        m_mat_camera_matrix.at<double>(1,1)=m_i_stereoscopic_view_res_y/2.0;
        m_mat_camera_matrix.at<double>(1,2)=m_i_stereoscopic_view_res_y/2.0;
        m_mat_camera_matrix.at<double>(2,0)=0;
        m_mat_camera_matrix.at<double>(2,1)=0;
        m_mat_camera_matrix.at<double>(2,2)=1;
        m_mat_distortion_coefficients=cv::Mat(4,1,cv::DataType<double>::type);
        m_mat_distortion_coefficients.at<double>(0,0)=m_prp_stereoscopic_dist->getFloat(); // Update k1 to enable barrel distortion
        m_mat_distortion_coefficients.at<double>(1,0)=0.0;
        m_mat_distortion_coefficients.at<double>(2,0)=0.0;
        m_mat_distortion_coefficients.at<double>(3,0)=0.0;

        cv::initUndistortRectifyMap(m_mat_camera_matrix,
                                    m_mat_distortion_coefficients,
                                    cv::Mat(),
                                    m_mat_camera_matrix,
                                    cv::Size(m_i_stereoscopic_view_res_x,m_i_stereoscopic_view_res_y),
                                    CV_32FC1,
                                    m_mat_undistort_map_1,
                                    m_mat_undistort_map_2);
    }
    void RVizPluginDisplayStereoscopic::UpdateStereoscopicView()
    {
        m_i_stereoscopic_view_res_x=m_ogr_render_texture_eye_left->getWidth();
        m_i_stereoscopic_view_res_y=m_ogr_render_texture_eye_left->getHeight();

        Ogre::PixelFormat ogr_pf_pixel_format=Ogre::PF_BYTE_RGB;
        uint pixelsize=Ogre::PixelUtil::getNumElemBytes(ogr_pf_pixel_format);
        uint uc_data_size=m_i_stereoscopic_view_res_x*m_i_stereoscopic_view_res_y*pixelsize;

        uchar* uc_data_eye_left=OGRE_ALLOC_T(uchar,static_cast<int>(uc_data_size),Ogre::MEMCATEGORY_RENDERSYS);
        uchar* uc_data_eye_right=OGRE_ALLOC_T(uchar,static_cast<int>(uc_data_size),Ogre::MEMCATEGORY_RENDERSYS);
        Ogre::PixelBox ogr_pb_pixelbox_eye_left(m_i_stereoscopic_view_res_x,m_i_stereoscopic_view_res_y,1,ogr_pf_pixel_format,uc_data_eye_left);
        Ogre::PixelBox ogr_pb_pixelbox_eye_right(m_i_stereoscopic_view_res_x,m_i_stereoscopic_view_res_y,1,ogr_pf_pixel_format,uc_data_eye_right);

        m_ogr_render_texture_eye_left->copyContentsToMemory(ogr_pb_pixelbox_eye_left,Ogre::RenderTarget::FB_AUTO);
        m_ogr_render_texture_eye_right->copyContentsToMemory(ogr_pb_pixelbox_eye_right,Ogre::RenderTarget::FB_AUTO);

        memcpy(&mat_img_stereoscopic_eye_left->data[0],uc_data_eye_left,uc_data_size);
        memcpy(&mat_img_stereoscopic_eye_right->data[0],uc_data_eye_right,uc_data_size);

        cv::remap(*mat_img_stereoscopic_eye_left,*mat_img_stereoscopic_eye_left_dist,m_mat_undistort_map_1,m_mat_undistort_map_2,cv::INTER_LINEAR);
        cv::remap(*mat_img_stereoscopic_eye_right,*mat_img_stereoscopic_eye_right_dist,m_mat_undistort_map_1,m_mat_undistort_map_2,cv::INTER_LINEAR);
        cv::hconcat(*mat_img_stereoscopic_eye_left_dist,*mat_img_stereoscopic_eye_right_dist,*mat_img_stereoscopic_portable);

        m_msg_img_stereoscopic_portable=cv_bridge::CvImage(std_msgs::msg::Header(),"rgb8",*mat_img_stereoscopic_portable).toImageMsg();
        m_pub_img_stereoscopic_portable.publish(m_msg_img_stereoscopic_portable);

        OGRE_FREE(uc_data_eye_left,Ogre::MEMCATEGORY_RENDERSYS);
        OGRE_FREE(uc_data_eye_right,Ogre::MEMCATEGORY_RENDERSYS);
    }

    // Initialize static properties
    int RVizPluginDisplayStereoscopic::si_resource_name_count;

    // Default constructor and destructor
    RVizPluginDisplayStereoscopic::RVizPluginDisplayStereoscopic()
    {
        // Do nothing for now...
    }
    RVizPluginDisplayStereoscopic::~RVizPluginDisplayStereoscopic()
    {
        CleanupStereoscopicTexture();
    }

}  // namespace rviz_plugin_display_stereoscopic

#include<pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_display_stereoscopic::RVizPluginDisplayStereoscopic,rviz_common::Display)
