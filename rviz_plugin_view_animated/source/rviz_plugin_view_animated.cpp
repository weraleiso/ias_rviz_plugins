#include<rviz_plugin_view_animated.hpp>



namespace rviz_plugin_view_animated
{
    // Inherited methods
    void RVizPluginViewAnimated::onInitialize()
    {
        // Initialize ROS2 properties
        m_hdl_node_rviz=context_->getRosNodeAbstraction().lock();
        m_hdl_node=m_hdl_node_rviz->get_raw_node();

        // Initialize helper properties
        si_resource_name_count++;
        QPixmap qpi_mouse_control_disabled("/opt/ros/jazzy/share/rviz_common/icons/forbidden.svg");
        m_qcu_mouse_control_disabled=QCursor(qpi_mouse_control_disabled,0,0);
        m_s_rviz_plugin_namespace="rviz_plugin_view_animated_"+std::to_string(si_resource_name_count)+"/";
        m_f_view_pause=0.0;
        m_f_view_elapsed=0.0;
        m_f_view_distance=0.0;
        m_f_frames_rendered=0.0;
        m_i_view_diff_x=0;
        m_i_view_diff_y=0;
        m_b_mouse_drag_enabled=false;

        // Initialize RViz properties
        m_prp_view_tf_frame=std::make_unique<rviz_common::properties::TfFrameProperty>("TF Frame (Local)",rviz_common::properties::TfFrameProperty::FIXED_FRAME_STRING,"TF Frame (Global).",this,(rviz_common::FrameManager*)context_->getFrameManager(),true,SLOT(cb_prp_update_view_tf_frame()));
        m_prp_view_mode=std::make_unique<rviz_common::properties::EditableEnumProperty>("Mode","ORBIT","Mode.",this);
        m_prp_view_mode->addOptionStd("ORBIT");
        m_prp_view_mode->addOptionStd("FPS");
        m_prp_view_speed_profile=std::make_unique<rviz_common::properties::EditableEnumProperty>("Speed Profile","DECLINING","Speed Profile.",this);
        m_prp_view_speed_profile->addOptionStd("DECLINING");
        m_prp_view_speed_profile->addOptionStd("RISING");
        m_prp_view_speed_profile->addOptionStd("WAVE");
        m_prp_view_speed_profile->addOptionStd("FULL");
        m_prp_view_speed_profile->addOptionStd("QUADRATIC");
        m_prp_view_speed_profile->addOptionStd("EXPONENTIAL");
        m_prp_view_duration=std::make_unique<rviz_common::properties::FloatProperty>("Duration",3.0,"Duration.",this);
        m_prp_view_duration->setMin(0.1);
        m_prp_view_progress=std::make_unique<rviz_common::properties::FloatProperty>("Progress",0.0,"Progress.",this);
        m_prp_view_progress->setReadOnly(true);
        m_prp_view_target_fps=std::make_unique<rviz_common::properties::FloatProperty>("FPS",20.0,"FPS.",this);
        m_prp_view_eye_local=std::make_unique<rviz_common::properties::VectorProperty>("Eye (Local)",3.0*Ogre::Vector3::UNIT_SCALE,"Eye (Local).",this,SLOT(cb_prp_update_view_eye_local()));
        m_prp_view_focus_local=std::make_unique<rviz_common::properties::VectorProperty>("Focus (Local)",Ogre::Vector3::ZERO,"Focus (Local).",this,SLOT(cb_prp_update_view_focus_local()));
        m_prp_view_up_local=std::make_unique<rviz_common::properties::VectorProperty>("Up (Local)",Ogre::Vector3::UNIT_Z,"Up (Local).",this,SLOT(cb_prp_update_view_up_local()));
        m_prp_view_eye=std::make_unique<rviz_common::properties::VectorProperty>("Eye (Fixed)",3.0*Ogre::Vector3::UNIT_SCALE,"Eye (Fixed).",this,SLOT(cb_prp_update_view_eye()));
        m_prp_view_focus=std::make_unique<rviz_common::properties::VectorProperty>("Focus (Fixed)",Ogre::Vector3::ZERO,"Focus (Fixed).",this,SLOT(cb_prp_update_view_focus()));
        m_prp_view_up=std::make_unique<rviz_common::properties::VectorProperty>("Up Axis (Fixed)",Ogre::Vector3::UNIT_Z,"Up Axis (Fixed).",this,SLOT(cb_prp_update_view_up()));
        m_prp_view_direction=std::make_unique<rviz_common::properties::VectorProperty>("Direction",Ogre::Vector3::ZERO-Ogre::Vector3::UNIT_SCALE,"Direction.",this,SLOT(cb_prp_update_view_direction()));
        m_prp_view_direction->setReadOnly(true);
        m_prp_view_orientation=std::make_unique<rviz_common::properties::QuaternionProperty>("Orientation",Ogre::Quaternion(1.0,0.0,0.0,0.0),"Orientation.",this,SLOT(cb_prp_update_view_orientation()));
        m_prp_view_orientation->setReadOnly(true);
        m_prp_view_distance=std::make_unique<rviz_common::properties::FloatProperty>("Distance",1.0,"Distance.",this,SLOT(cb_prp_update_view_distance()));
        m_prp_view_distance->setMin(0.1);
        m_prp_view_distance->setReadOnly(true);
        m_prp_view_frame_by_frame_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Frame By Frame Enabled",false,"Frame By Frame Enabled.",this);
        m_prp_view_vector_up_lock_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Up Vector Lock Enabled",true,"Up Vector Lock Enabled.",this,SLOT(cb_prp_update_view_up_lock_enabled()));
        m_prp_view_pose_publish_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Pose Publish Enabled",false,"Pose Publish Enabled.",this);
        m_prp_view_mouse_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Mouse Control Enabled",true,"Mouse Control Enabled.",this);
        m_prp_view_animation_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Animation Enabled",false,"Animation Enabled.",this);
        m_prp_view_live_publish_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Live Publish Enabled",false,"Live Publish Enabled.",this,SLOT(cb_prp_update_view_live_enable()));
        m_prp_view_live_record_enabled=std::make_unique<rviz_common::properties::BoolProperty>("Live Record Enabled",false,"Live Record Enabled.",this,SLOT(cb_prp_update_view_live_record_enable()));
        m_prp_view_live_record_path=std::make_unique<rviz_common::properties::StringProperty>("Live Record Path","/home/ias/Videos/","Live Record Path.",this);
        m_prp_view_live_width=std::make_unique<rviz_common::properties::IntProperty>("Live Resolution X",1280,"Live Resolution X.",this);
        m_prp_view_live_height=std::make_unique<rviz_common::properties::IntProperty>("Live Resolution Y",720,"Live Resolution Y.",this);
        m_prp_view_live_color=std::make_unique<rviz_common::properties::ColorProperty>("Live Background Color",QColor(48,48,48),"Live Background Color.",this,SLOT(cb_prp_update_view_live_color()));
        m_prp_view_live_topic=std::make_unique<rviz_common::properties::RosTopicProperty>("Live Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"live/compressed"),"sensor_msgs/msg/CompressedImage","Live Topic.",this,SLOT(cb_prp_update_view_live_topic()));
        m_prp_view_live_topic->initialize(m_hdl_node_rviz);
        m_prp_view_pose_topic=std::make_unique<rviz_common::properties::RosTopicProperty>("Pose Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"pose"),"geometry_msgs/msg/PoseStamped","Pose Topic.",this,SLOT(cb_prp_update_view_pose_topic()));
        m_prp_view_pose_topic->initialize(m_hdl_node_rviz);
        m_prp_view_animation_completed_topic=std::make_unique<rviz_common::properties::RosTopicProperty>("Completed Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"completed"),"std_msgs/msg/Bool","Completed Topic.",this,SLOT(cb_prp_update_view_animation_completed_topic()));
        m_prp_view_animation_completed_topic->initialize(m_hdl_node_rviz);
        m_prp_view_animation_pause_topic=std::make_unique<rviz_common::properties::RosTopicProperty>("Pause Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"pause"),"std_msgs/msg/Float64","Pause Topic.",this,SLOT(cb_prp_update_view_animation_pause_topic()));
        m_prp_view_animation_pause_topic->initialize(m_hdl_node_rviz);
        m_prp_view_trajectory_topic=std::make_unique<rviz_common::properties::RosTopicProperty>("Trajectory Topic",QString::fromStdString(m_s_rviz_plugin_namespace+"trajectory"),"rviz_plugin_view_animated_msgs/msg/ViewTrajectory","Trajectory Topic.",this,SLOT(cb_prp_update_view_trajectory_topic()));
        m_prp_view_trajectory_topic->initialize(m_hdl_node_rviz);

        // Initialize view focal point
        m_shp_focal=std::make_unique<rviz_rendering::Shape>(rviz_rendering::Shape::Type::Sphere,context_->getSceneManager(),context_->getSceneManager()->getRootSceneNode());
        m_shp_focal->setScale(Ogre::Vector3(0.1f,0.1f,0.05f));
        m_shp_focal->setColor(1.0f,1.0f,0.0f,1.0f);
        m_shp_focal->getRootNode()->setVisible(true);

        RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_view_animated::RVizPluginViewAnimated - Initialized!");
    }
    void RVizPluginViewAnimated::onActivate()
    {
        UpdateViewTopicsAndTransports();
        UpdateViewPose();
        InitializeViewLiveTexture();
    }
    void RVizPluginViewAnimated::reset()
    {
        m_prp_view_mode->setStdString("ORBIT");
        m_prp_view_speed_profile->setStdString("DECLINING");
        m_prp_view_duration->setFloat(3.0);
        m_prp_view_progress->setFloat(0.0);
        m_prp_view_target_fps->setFloat(20.0);
        m_prp_view_frame_by_frame_enabled->setBool(false);
        m_prp_view_vector_up_lock_enabled->setBool(true);
        m_prp_view_pose_publish_enabled->setBool(false);
        m_prp_view_mouse_enabled->setBool(true);
        m_prp_view_animation_enabled->setBool(false);
        m_prp_view_live_publish_enabled->setBool(false);
        m_prp_view_live_record_enabled->setBool(false);
        m_prp_view_live_color->setColor(QColor(48,48,48));

        m_prp_view_eye_local->setVector(3.0*Ogre::Vector3::UNIT_SCALE);
        m_prp_view_focus_local->setVector(Ogre::Vector3::ZERO);
        m_prp_view_up_local->setVector(Ogre::Vector3::UNIT_Z);
        cb_prp_update_view_tf_frame();

        m_shp_focal->setPosition(m_prp_view_focus->getVector());
        m_shp_focal->setScale(Ogre::Vector3(0.1f,0.1f,0.05f));
        m_shp_focal->setColor(1.0f,1.0f,0.0f,1.0f);
    }
    void RVizPluginViewAnimated::update(float dt,float ros_dt)
    {
        // Update TF Frame
        if(context_->getFrameManager()->getTransform(m_prp_view_tf_frame->getFrameStd(),m_ov3_tf_frame_position,m_oqu_tf_frame_orientation))
        {
            // Process movements, if still >=2 are available
            if(m_prp_view_animation_enabled->getBool() && m_vec_vmo_movements.size()>=2)
            {
                if(m_f_view_pause>0.0)
                {
                    rclcpp::sleep_for(std::chrono::nanoseconds(long(m_f_view_pause*1000000000)));
                    m_tim_view_animation_start+=rclcpp::Duration::from_seconds(m_f_view_pause);
                    m_f_view_pause=0.0;
                }

                // Load next pair of movements as origin and goal
                rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_origin=m_vec_vmo_movements.front();
                rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_goal=m_vec_vmo_movements.begin()[1];

                // Update all properties for the next goal movement
                m_prp_view_tf_frame->setStdString(msg_vmo_goal.view_tf_frame);
                if(msg_vmo_goal.view_mode==0) m_prp_view_mode->setStdString("ORBIT");
                if(msg_vmo_goal.view_mode==1) m_prp_view_mode->setStdString("FPS");
                if(msg_vmo_goal.view_speed_profile==0) m_prp_view_speed_profile->setStdString("DECLINING");
                if(msg_vmo_goal.view_speed_profile==1) m_prp_view_speed_profile->setStdString("RISING");
                if(msg_vmo_goal.view_speed_profile==2) m_prp_view_speed_profile->setStdString("WAVE");
                if(msg_vmo_goal.view_speed_profile==3) m_prp_view_speed_profile->setStdString("FULL");
                if(msg_vmo_goal.view_speed_profile==4) m_prp_view_speed_profile->setStdString("QUADRATIC");
                if(msg_vmo_goal.view_speed_profile==5) m_prp_view_speed_profile->setStdString("EXPONENTIAL");
                m_prp_view_duration->setFloat(msg_vmo_goal.view_duration.data);
                m_prp_view_target_fps->setFloat(msg_vmo_goal.view_target_fps.data);
                m_prp_view_frame_by_frame_enabled->setBool(msg_vmo_goal.view_frame_by_frame_enabled);
                m_prp_view_vector_up_lock_enabled->setBool(msg_vmo_goal.view_vector_up_lock_enabled);
                m_prp_view_pose_publish_enabled->setBool(msg_vmo_goal.view_pose_enabled);
                m_prp_view_live_publish_enabled->setBool(msg_vmo_goal.view_live_enabled);
                m_prp_view_mouse_enabled->setBool(msg_vmo_goal.view_mouse_enabled);

                float f_view_movement_progress=0.0;
                if(m_prp_view_frame_by_frame_enabled->getBool())
                {
                    f_view_movement_progress=m_f_frames_rendered/(m_prp_view_target_fps->getFloat()*msg_vmo_goal.view_duration.data);
                    m_f_frames_rendered=m_f_frames_rendered+1.0;
                }
                else
                {
                    rclcpp::Duration dur_elapsed=m_hdl_node->get_clock()->now()-m_tim_view_animation_start;
                    f_view_movement_progress=dur_elapsed.seconds()/msg_vmo_goal.view_duration.data;
                }

                bool b_movement_completed=false;
                if(f_view_movement_progress>=1.0)
                {
                    f_view_movement_progress=1.0;
                    b_movement_completed=true;
                }
                m_prp_view_progress->setValue(f_view_movement_progress*msg_vmo_goal.view_duration.data);

                // Update camera pose based on calculated iterations
                float f_view_animation_iteration=CalculateViewAnimationIteration(f_view_movement_progress,msg_vmo_goal.view_speed_profile);
                Ogre::Vector3 ov3_eye_new;
                ov3_eye_new.x=msg_vmo_origin.view_eye.point.x+f_view_animation_iteration*(msg_vmo_goal.view_eye.point.x-msg_vmo_origin.view_eye.point.x);
                ov3_eye_new.y=msg_vmo_origin.view_eye.point.y+f_view_animation_iteration*(msg_vmo_goal.view_eye.point.y-msg_vmo_origin.view_eye.point.y);
                ov3_eye_new.z=msg_vmo_origin.view_eye.point.z+f_view_animation_iteration*(msg_vmo_goal.view_eye.point.z-msg_vmo_origin.view_eye.point.z);
                m_prp_view_eye->setVector(ov3_eye_new);

                if(m_prp_view_mode->getStdString()=="ORBIT")
                {
                    // In ORBIT mode derive view direction based on eye and fixed focus points!
                    Ogre::Vector3 ov3_focus_new;
                    ov3_focus_new.x=msg_vmo_origin.view_focus.point.x+f_view_animation_iteration*(msg_vmo_goal.view_focus.point.x-msg_vmo_origin.view_focus.point.x);
                    ov3_focus_new.y=msg_vmo_origin.view_focus.point.y+f_view_animation_iteration*(msg_vmo_goal.view_focus.point.y-msg_vmo_origin.view_focus.point.y);
                    ov3_focus_new.z=msg_vmo_origin.view_focus.point.z+f_view_animation_iteration*(msg_vmo_goal.view_focus.point.z-msg_vmo_origin.view_focus.point.z);
                    m_prp_view_focus->setVector(ov3_focus_new);
                }
                else if(m_prp_view_mode->getStdString()=="FPS")
                {
                    // In FPS mode derive view direction directly from focus vector!
                    m_prp_view_focus->setVector(Ogre::Vector3(msg_vmo_goal.view_focus.point.x,msg_vmo_goal.view_focus.point.y,msg_vmo_goal.view_focus.point.z));
                }
                else
                {
                    // Do nothing for now...
                }

                Ogre::Vector3 vc3_up_new;
                vc3_up_new.x=msg_vmo_origin.view_up.vector.x+f_view_animation_iteration*(msg_vmo_goal.view_up.vector.x-msg_vmo_origin.view_up.vector.x);
                vc3_up_new.y=msg_vmo_origin.view_up.vector.y+f_view_animation_iteration*(msg_vmo_goal.view_up.vector.y-msg_vmo_origin.view_up.vector.y);
                vc3_up_new.z=msg_vmo_origin.view_up.vector.z+f_view_animation_iteration*(msg_vmo_goal.view_up.vector.z-msg_vmo_origin.view_up.vector.z);
                m_prp_view_up->setVector(vc3_up_new);

                UpdateViewPose();

                if(b_movement_completed)
                {
                    // Delete current start element in buffer
                    m_vec_vmo_movements.erase(m_vec_vmo_movements.begin());
                    if(m_vec_vmo_movements.size()>=2)
                    {
                        m_tim_view_animation_start+=rclcpp::Duration::from_seconds(msg_vmo_goal.view_duration.data);
                        m_f_frames_rendered=0.0;
                    }
                    else
                    {
                        UpdateViewCancel();
                    }
                }
            }
        }
        else
        {
            // Do nothing for now...
        }

        PublishViewPose();
        PublishViewLiveImage();
    }
    void RVizPluginViewAnimated::handleMouseEvent(rviz_common::ViewportMouseEvent& event)
    {
        if(m_prp_view_mouse_enabled->getBool()==false)
        {
            setCursor(m_qcu_mouse_control_disabled);
            return;
        }
        else if(event.shift())
        {
            // Do nothing for now...
        }
        else if(event.control())
        {
            // Do nothing for now...
        }
        else
        {
            // Do nothing for now...
        }

        if(event.type==QEvent::MouseButtonPress)
        {
            UpdateViewCancel();
            m_b_mouse_drag_enabled=true;
        }
        else if(event.type==QEvent::MouseButtonRelease)
        {
            m_b_mouse_drag_enabled=false;
        }
        else if(m_b_mouse_drag_enabled && event.type==QEvent::MouseMove)
        {
            m_i_view_diff_x=event.x-event.last_x;
            m_i_view_diff_y=event.y-event.last_y;
        }
        else
        {
            // Do nothing for now...
        }

        // Left-button drag
        if(event.left() && !event.shift())
        {
            setCursor(Rotate3D);
            UpdateViewYawPitchRoll(m_i_view_diff_x*0.005,m_i_view_diff_y*0.005,0.0);
        } // Middle-button
        else if(event.middle())
        {
            setCursor(MoveXY);
            if(m_prp_view_mode->getStdString()=="ORBIT")
            {
                float fovY=camera_->getFOVy().valueRadians();
                float fovX=2.0f*atan(tan(fovY/2.0f)*camera_->getAspectRatio());

                int width=camera_->getViewport()->getActualWidth();
                int height=camera_->getViewport()->getActualHeight();

                UpdateViewEyeFocus( -((float)m_i_view_diff_x/(float)width)*m_f_view_distance*tan(fovX/2.0f)*2.0f,
                                    ((float)m_i_view_diff_y/(float)height)*m_f_view_distance*tan(fovY/2.0f)*2.0f,
                                    0.0f);
            }
            else if(m_prp_view_mode->getStdString()=="FPS")
            {
                UpdateViewEyeFocus(-m_i_view_diff_x*0.01f,m_i_view_diff_y*0.01f,0.0f);
            }
        } // Right-button
        else if(event.right())
        {
            if(event.shift()||(m_prp_view_mode->getStdString()=="ORBIT"))
            {
                setCursor(Zoom);
                UpdateViewEye(0.0f,0.0f,m_i_view_diff_y*0.01f*m_f_view_distance);
            }
            if(event.shift()||(m_prp_view_mode->getStdString()=="FPS"))
            {
                setCursor(MoveZ);
                UpdateViewEyeFocus(0.0f,0.0f,m_i_view_diff_y*0.01f*m_f_view_distance);
            }
            else
            {
                // Do nothing for now...
            }
        }
        else
        {
            setCursor(event.shift() ? MoveXY : Rotate3D);
        } // Mouse wheel
        if(event.wheel_delta!=0)
        {
            setCursor(Zoom);
            int diff=event.wheel_delta;
            if(event.shift())
            {
                UpdateViewEyeFocus(0.0f,0.0f,-diff*0.001f*m_f_view_distance);
            }
            else if(event.control())
            {
                UpdateViewYawPitchRoll(0.0f,0.0f,diff*0.001f);
            }
            else
            {
                UpdateViewEye(0.0f,0.0f,-diff*0.001f*m_f_view_distance);
            }
        }
        if(event.type==QEvent::MouseButtonPress && event.left() && event.control() && event.shift())
        {
            if(m_prp_view_mode->getStdString()=="ORBIT")
            {
                m_prp_view_mode->setStdString("FPS");
                m_ov3_focus_old=m_prp_view_focus->getVector();
                m_ov3_focus_local_old=m_prp_view_focus_local->getVector();
                m_prp_view_focus->setVector(m_prp_view_direction->getVector());
            }
            else if(m_prp_view_mode->getStdString()=="FPS")
            {
                m_prp_view_mode->setStdString("ORBIT");
                m_prp_view_focus->setVector(m_ov3_focus_old);
                m_prp_view_focus_local->setVector(m_ov3_focus_local_old);
            }
            else
            {
                // Do nothing for now...
            }
            UpdateViewPose();
        }
    }
    void RVizPluginViewAnimated::transitionFrom(ViewController* previous_view)
    {
        // Do nothing for now...
    }
    void RVizPluginViewAnimated::lookAt(const Ogre::Vector3& point)
    {
        // Only "look at" in ORBIT mode!
        if(m_prp_view_mouse_enabled->getBool()==true)
        {
            UpdateViewInitialize();

            if(m_prp_view_mode->getStdString()=="ORBIT")
            {
                rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement;
                msg_vmo_movement.view_tf_frame=m_prp_view_tf_frame->getFrameStd();
                if(m_prp_view_mode->getStdString()=="ORBIT") msg_vmo_movement.view_mode=0;
                if(m_prp_view_mode->getStdString()=="FPS") msg_vmo_movement.view_mode=1;
                if(m_prp_view_speed_profile->getStdString()=="DECLINING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::DECLINING;
                if(m_prp_view_speed_profile->getStdString()=="RISING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::RISING;
                if(m_prp_view_speed_profile->getStdString()=="WAVE") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::WAVE;
                if(m_prp_view_speed_profile->getStdString()=="FULL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::FULL;
                if(m_prp_view_speed_profile->getStdString()=="QUADRATIC") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::QUADRATIC;
                if(m_prp_view_speed_profile->getStdString()=="EXPONENTIAL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::EXPONENTIAL;
                msg_vmo_movement.view_duration.data=m_prp_view_duration->getFloat();
                msg_vmo_movement.view_target_fps.data=m_prp_view_target_fps->getFloat();
                msg_vmo_movement.view_frame_by_frame_enabled=m_prp_view_frame_by_frame_enabled->getBool();
                msg_vmo_movement.view_vector_up_lock_enabled=m_prp_view_vector_up_lock_enabled->getBool();
                msg_vmo_movement.view_pose_enabled=m_prp_view_pose_publish_enabled->getBool();
                msg_vmo_movement.view_live_enabled=m_prp_view_live_publish_enabled->getBool();
                msg_vmo_movement.view_mouse_enabled=m_prp_view_mouse_enabled->getBool();
                msg_vmo_movement.view_eye.point.x=m_prp_view_eye->getVector().x;
                msg_vmo_movement.view_eye.point.y=m_prp_view_eye->getVector().y;
                msg_vmo_movement.view_eye.point.z=m_prp_view_eye->getVector().z;
                msg_vmo_movement.view_focus.point.x=point.x;
                msg_vmo_movement.view_focus.point.y=point.y;
                msg_vmo_movement.view_focus.point.z=point.z;
                msg_vmo_movement.view_up.vector.x=m_prp_view_up->getVector().x;
                msg_vmo_movement.view_up.vector.y=m_prp_view_up->getVector().y;
                msg_vmo_movement.view_up.vector.z=m_prp_view_up->getVector().z;
                m_vec_vmo_movements.push_back(msg_vmo_movement);
            }
            else if(m_prp_view_mode->getStdString()=="FPS")
            {
                rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement;
                msg_vmo_movement.view_tf_frame=m_prp_view_tf_frame->getFrameStd();
                if(m_prp_view_mode->getStdString()=="ORBIT") msg_vmo_movement.view_mode=0;
                if(m_prp_view_mode->getStdString()=="FPS") msg_vmo_movement.view_mode=1;
                if(m_prp_view_speed_profile->getStdString()=="DECLINING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::DECLINING;
                if(m_prp_view_speed_profile->getStdString()=="RISING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::RISING;
                if(m_prp_view_speed_profile->getStdString()=="WAVE") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::WAVE;
                if(m_prp_view_speed_profile->getStdString()=="FULL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::FULL;
                if(m_prp_view_speed_profile->getStdString()=="QUADRATIC") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::QUADRATIC;
                if(m_prp_view_speed_profile->getStdString()=="EXPONENTIAL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::EXPONENTIAL;
                msg_vmo_movement.view_duration.data=m_prp_view_duration->getFloat();
                msg_vmo_movement.view_target_fps.data=m_prp_view_target_fps->getFloat();
                msg_vmo_movement.view_frame_by_frame_enabled=m_prp_view_frame_by_frame_enabled->getBool();
                msg_vmo_movement.view_vector_up_lock_enabled=m_prp_view_vector_up_lock_enabled->getBool();
                msg_vmo_movement.view_pose_enabled=m_prp_view_pose_publish_enabled->getBool();
                msg_vmo_movement.view_live_enabled=m_prp_view_live_publish_enabled->getBool();
                msg_vmo_movement.view_mouse_enabled=m_prp_view_mouse_enabled->getBool();
                msg_vmo_movement.view_eye.point.x=point.x;
                msg_vmo_movement.view_eye.point.y=point.y;
                msg_vmo_movement.view_eye.point.z=point.z;
                msg_vmo_movement.view_focus.point.x=m_prp_view_focus->getVector().x;
                msg_vmo_movement.view_focus.point.y=m_prp_view_focus->getVector().y;
                msg_vmo_movement.view_focus.point.z=m_prp_view_focus->getVector().z;
                msg_vmo_movement.view_up.vector.x=m_prp_view_up->getVector().x;
                msg_vmo_movement.view_up.vector.y=m_prp_view_up->getVector().y;
                msg_vmo_movement.view_up.vector.z=m_prp_view_up->getVector().z;
                m_vec_vmo_movements.push_back(msg_vmo_movement);
            }
            else
            {
                // Do nothing...
            }
        }
    }

    // Callbacks to process ROS2 messages
    void RVizPluginViewAnimated::cb_sub_vtr_view_trajectory(rviz_plugin_view_animated_msgs::msg::ViewTrajectory msg_vtr_trajectory)
    {
        if(msg_vtr_trajectory.view_trajectory.empty()==false)
        {
            // Add proper initial trajectory view movement based on current camera pose
            m_prp_view_tf_frame->setStdString(msg_vtr_trajectory.view_trajectory[0].view_tf_frame);
            update(0.0,0.0);
            rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement_init;
            msg_vmo_movement_init.view_tf_frame=msg_vtr_trajectory.view_trajectory[0].view_tf_frame;
            msg_vmo_movement_init.view_mode=msg_vtr_trajectory.view_trajectory[0].view_mode;
            msg_vmo_movement_init.view_speed_profile=msg_vtr_trajectory.view_trajectory[0].view_speed_profile;
            msg_vmo_movement_init.view_duration.data=msg_vtr_trajectory.view_trajectory[0].view_duration.data;
            msg_vmo_movement_init.view_target_fps.data=msg_vtr_trajectory.view_trajectory[0].view_target_fps.data;
            msg_vmo_movement_init.view_frame_by_frame_enabled=msg_vtr_trajectory.view_trajectory[0].view_frame_by_frame_enabled;
            msg_vmo_movement_init.view_vector_up_lock_enabled=msg_vtr_trajectory.view_trajectory[0].view_vector_up_lock_enabled;
            msg_vmo_movement_init.view_pose_enabled=msg_vtr_trajectory.view_trajectory[0].view_pose_enabled;
            msg_vmo_movement_init.view_live_enabled=msg_vtr_trajectory.view_trajectory[0].view_live_enabled;
            msg_vmo_movement_init.view_mouse_enabled=msg_vtr_trajectory.view_trajectory[0].view_mouse_enabled;
            msg_vmo_movement_init.view_eye.point.x=camera_->getPosition().x;
            msg_vmo_movement_init.view_eye.point.y=camera_->getPosition().y;
            msg_vmo_movement_init.view_eye.point.z=camera_->getPosition().z;
            msg_vmo_movement_init.view_focus.point.x=msg_vtr_trajectory.view_trajectory[0].view_focus.point.x;
            msg_vmo_movement_init.view_focus.point.y=msg_vtr_trajectory.view_trajectory[0].view_focus.point.y;
            msg_vmo_movement_init.view_focus.point.z=msg_vtr_trajectory.view_trajectory[0].view_focus.point.z;
            msg_vmo_movement_init.view_up.vector.x=msg_vtr_trajectory.view_trajectory[0].view_up.vector.x;
            msg_vmo_movement_init.view_up.vector.y=msg_vtr_trajectory.view_trajectory[0].view_up.vector.y;
            msg_vmo_movement_init.view_up.vector.z=msg_vtr_trajectory.view_trajectory[0].view_up.vector.z;
            m_vec_vmo_movements.push_back(msg_vmo_movement_init);

            for(int i=0;i<msg_vtr_trajectory.view_trajectory.size();i++)
            {
                rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement=msg_vtr_trajectory.view_trajectory[i];
                if(msg_vmo_movement.view_tf_frame=="") msg_vmo_movement.view_tf_frame="ias_wor_universe";
                if(msg_vmo_movement.view_mode<0) msg_vmo_movement.view_mode=0;
                if(msg_vmo_movement.view_mode>1) msg_vmo_movement.view_mode=1;
                if(msg_vmo_movement.view_speed_profile<0) msg_vmo_movement.view_speed_profile=0;
                if(msg_vmo_movement.view_speed_profile>5) msg_vmo_movement.view_speed_profile=5;
                if(msg_vmo_movement.view_duration.data<0.1) msg_vmo_movement.view_duration.data=0.1;
                TransformViewTrajectoryToFixedTFFrame(msg_vmo_movement.view_eye,
                                                      msg_vmo_movement.view_focus,
                                                      msg_vmo_movement.view_up,
                                                      msg_vmo_movement.view_mode);
                m_vec_vmo_movements.push_back(msg_vmo_movement);
            }

            m_tim_view_animation_start=m_hdl_node->get_clock()->now();
            m_prp_view_animation_enabled->setBool(true);
        }
    }
    void RVizPluginViewAnimated::cb_sub_f32_view_pause(std_msgs::msg::Float32 msg_f32_pause)
    {
        m_f_view_pause=msg_f32_pause.data;
    }

    // Callbacks to process property updates
    void RVizPluginViewAnimated::cb_prp_update_view_tf_frame()
    {
        cb_prp_update_view_eye_local();
        cb_prp_update_view_focus_local();
        cb_prp_update_view_up_local();
    }
    void RVizPluginViewAnimated::cb_prp_update_view_eye_local()
    {
        Ogre::Vector3 ov3_fixed=m_prp_view_eye_local->getVector();
        context_->getFrameManager()->getTransform(m_prp_view_tf_frame->getFrameStd(),m_ov3_tf_frame_position,m_oqu_tf_frame_orientation);
        m_prp_view_eye->setVector(AttachedLocalToFixedFrame(ov3_fixed));
    }
    void RVizPluginViewAnimated::cb_prp_update_view_focus_local()
    {
        Ogre::Vector3 ov3_fixed=m_prp_view_focus_local->getVector();
        context_->getFrameManager()->getTransform(m_prp_view_tf_frame->getFrameStd(),m_ov3_tf_frame_position,m_oqu_tf_frame_orientation);
        if(m_prp_view_mode->getString()=="ORBIT")
        {
            m_prp_view_focus->setVector(AttachedLocalToFixedFrame(ov3_fixed));
        }
        else if(m_prp_view_mode->getString()=="FPS")
        {
            m_prp_view_focus->setVector(m_oqu_tf_frame_orientation*ov3_fixed);
        }
        else
        {
            // Do nothing...
        }
    }
    void RVizPluginViewAnimated::cb_prp_update_view_up_local()
    {
        Ogre::Vector3 ov3_fixed=m_prp_view_up_local->getVector();
        context_->getFrameManager()->getTransform(m_prp_view_tf_frame->getFrameStd(),m_ov3_tf_frame_position,m_oqu_tf_frame_orientation);
        m_prp_view_up->setVector(m_oqu_tf_frame_orientation*ov3_fixed);
    }
    void RVizPluginViewAnimated::cb_prp_update_view_eye()
    {
        UpdateViewPose();
    }
    void RVizPluginViewAnimated::cb_prp_update_view_focus()
    {
        UpdateViewPose();
    }
    void RVizPluginViewAnimated::cb_prp_update_view_up()
    {
        UpdateViewPose();
    }
    void RVizPluginViewAnimated::cb_prp_update_view_up_lock_enabled()
    {
        UpdateViewPose();
    }
    void RVizPluginViewAnimated::cb_prp_update_view_distance()
    {
        // Do nothing for now...
    }
    void RVizPluginViewAnimated::cb_prp_update_view_direction()
    {
        // Do nothing for now...
    }
    void RVizPluginViewAnimated::cb_prp_update_view_orientation()
    {
        // Do nothing for now...
    }
    void RVizPluginViewAnimated::cb_prp_update_view_live_enable()
    {
        // Do nothing for now...
    }
    void RVizPluginViewAnimated::cb_prp_update_view_live_record_enable()
    {
        if(m_prp_view_live_record_enabled->getBool()==true)
        {
            m_siz_view_live_frame.width=m_prp_view_live_width->getInt();
            m_siz_view_live_frame.height=m_prp_view_live_height->getInt();

            std::time_t tim_time_local=std::time(nullptr);
            std::tm localTime=*std::localtime(&tim_time_local);
            std::ostringstream sst_time;
            sst_time << std::put_time(&localTime,"%H_%M");
            std::string s_time_local=sst_time.str();

            m_vwt_view_live.open(m_prp_view_live_record_path->getStdString()+"rviz_plugin_view_animated_"+s_time_local+".avi",
                                 cv::VideoWriter::fourcc('M','J','P','G'),
                                 m_prp_view_target_fps->getFloat(),
                                 m_siz_view_live_frame,
                                 true);
        }
        else
        {
            if(m_vwt_view_live.isOpened()==true)
            {
                m_vwt_view_live.release();
            }
        }
    }
    void RVizPluginViewAnimated::cb_prp_update_view_live_color()
    {
        m_p_ret_view_live->getViewport(0)->setBackgroundColour(m_prp_view_live_color->getOgreColor());
    }
    void RVizPluginViewAnimated::cb_prp_update_view_live_topic()
    {
        m_pub_img_view_live.reset();
        m_pub_img_view_live=m_hdl_node->create_publisher<sensor_msgs::msg::CompressedImage>(m_prp_view_live_topic->getStdString(),1);
    }
    void RVizPluginViewAnimated::cb_prp_update_view_pose_topic()
    {
        m_pub_pst_view.reset();
        m_pub_pst_view=m_hdl_node->create_publisher<geometry_msgs::msg::PoseStamped>(m_prp_view_pose_topic->getStdString(),1);
    }
    void RVizPluginViewAnimated::cb_prp_update_view_animation_completed_topic()
    {
        m_pub_bol_view_animation_finished.reset();
        m_pub_bol_view_animation_finished=m_hdl_node->create_publisher<std_msgs::msg::Bool>(m_prp_view_animation_completed_topic->getStdString(),1);
    }
    void RVizPluginViewAnimated::cb_prp_update_view_animation_pause_topic()
    {
        m_sub_f32_view_pause.reset();
        m_sub_f32_view_pause=m_hdl_node->create_subscription<std_msgs::msg::Float32>(
                    m_prp_view_animation_pause_topic->getStdString(),
                    1,
                    std::bind(&RVizPluginViewAnimated::cb_sub_f32_view_pause,this,std::placeholders::_1));
    }
    void RVizPluginViewAnimated::cb_prp_update_view_trajectory_topic()
    {
        m_sub_vtr_view_trajectory.reset();
        m_sub_vtr_view_trajectory=m_hdl_node->create_subscription<rviz_plugin_view_animated_msgs::msg::ViewTrajectory>(
                    m_prp_view_trajectory_topic->getStdString(),
                    1,
                    std::bind(&RVizPluginViewAnimated::cb_sub_vtr_view_trajectory,this,std::placeholders::_1));
    }

    // Helper methods
    void RVizPluginViewAnimated::UpdateViewTopicsAndTransports()
    {
        // Update properties for subscriber/publisher topics
        m_prp_view_live_topic->setStdString(m_s_rviz_plugin_namespace+"live/compressed");
        m_prp_view_pose_topic->setStdString(m_s_rviz_plugin_namespace+"pose");
        m_prp_view_animation_completed_topic->setStdString(m_s_rviz_plugin_namespace+"completed");
        m_prp_view_animation_pause_topic->setStdString(m_s_rviz_plugin_namespace+"pause");
        m_prp_view_trajectory_topic->setStdString(m_s_rviz_plugin_namespace+"trajectory");

        // Initialize subscribers and publishers
        m_sub_vtr_view_trajectory.reset();
        m_sub_f32_view_pause.reset();
        m_pub_bol_view_animation_finished.reset();
        m_pub_pst_view.reset();
        m_pub_img_view_live.reset();
        m_sub_vtr_view_trajectory=m_hdl_node->create_subscription<rviz_plugin_view_animated_msgs::msg::ViewTrajectory>(
                    m_prp_view_trajectory_topic->getStdString(),
                    1,
                    std::bind(&RVizPluginViewAnimated::cb_sub_vtr_view_trajectory,this,std::placeholders::_1));
        m_sub_f32_view_pause=m_hdl_node->create_subscription<std_msgs::msg::Float32>(
                    m_prp_view_animation_pause_topic->getStdString(),
                    1,
                    std::bind(&RVizPluginViewAnimated::cb_sub_f32_view_pause,this,std::placeholders::_1));
        m_pub_bol_view_animation_finished=m_hdl_node->create_publisher<std_msgs::msg::Bool>(m_prp_view_animation_completed_topic->getStdString(),1);
        m_pub_pst_view=m_hdl_node->create_publisher<geometry_msgs::msg::PoseStamped>(m_prp_view_pose_topic->getStdString(),1);
        m_pub_img_view_live=m_hdl_node->create_publisher<sensor_msgs::msg::CompressedImage>(m_prp_view_live_topic->getStdString(),1);
        tf2_broadcaster=std::make_unique<tf2_ros::TransformBroadcaster>(m_hdl_node);
    }
    void RVizPluginViewAnimated::UpdateViewPose()
    {
        camera_->setPosition(m_prp_view_eye->getVector());
        camera_->setFixedYawAxis(m_prp_view_vector_up_lock_enabled->getBool(),m_prp_view_up->getVector());
        if(m_prp_view_mode->getStdString()=="ORBIT")
        {
            m_prp_view_direction->setVector(m_prp_view_focus->getVector()-m_prp_view_eye->getVector());
            m_f_view_distance=(m_prp_view_eye->getVector()-m_prp_view_focus->getVector()).length();
            m_prp_view_distance->setFloat(m_f_view_distance);

            m_shp_focal->setPosition(m_prp_view_focus->getVector());
            m_shp_focal->setScale(Ogre::Vector3(0.1f,0.1f,0.05f));
            m_shp_focal->setColor(1.0f,1.0f,0.0f,1.0f);
        }
        else if(m_prp_view_mode->getStdString()=="FPS")
        {
            m_prp_view_direction->setVector(m_prp_view_focus->getVector());

            m_shp_focal->setPosition(m_prp_view_eye->getVector()+m_prp_view_direction->getVector());
            m_shp_focal->setScale(Ogre::Vector3(0.025f,0.025f,0.0125f));
            m_shp_focal->setColor(0.0f,1.0f,1.0f,0.5f);
        }
        else
        {
            // Do nothing for now...
        }
        m_prp_view_orientation->setQuaternion(getQuaternionFromDirection(m_prp_view_direction->getVector(),m_prp_view_up->getVector()));
        camera_->setDirection(m_prp_view_direction->getVector());
    }
    void RVizPluginViewAnimated::UpdateViewYawPitchRoll(float f_yaw,float f_pitch,float f_roll)
    {
        if(m_prp_view_mode->getStdString()=="ORBIT")
        {
            Ogre::Quaternion oqu_view_current=camera_->getOrientation();
            Ogre::Quaternion oqu_yaw,oqu_pitch,oqu_roll;
            oqu_yaw.FromAngleAxis(Ogre::Radian(-1.0*f_yaw),Ogre::Vector3::UNIT_Y);
            oqu_pitch.FromAngleAxis(Ogre::Radian(-1.0*f_pitch),Ogre::Vector3::UNIT_X);
            oqu_roll.FromAngleAxis(Ogre::Radian(f_roll),Ogre::Vector3::UNIT_Z);
            Ogre::Quaternion oqu_view_new=oqu_view_current*oqu_yaw*oqu_pitch*oqu_roll;
            if(oqu_view_new.getPitch()<Ogre::Radian(5.0*M_PI/180.0) || oqu_view_new.getPitch()>Ogre::Radian(175.0*M_PI/180.0))
            {
                oqu_view_new=oqu_view_new*oqu_pitch.Inverse();
            }
            Ogre::Vector3 ov3_pos_eye_new=m_prp_view_focus->getVector()+m_f_view_distance*oqu_view_new.zAxis();
            m_prp_view_eye->setVector(ov3_pos_eye_new);
            // The m_prp_view_direction vector gets finally derived in the UpdateViewPose()
            // this updates the view properly!
        }
        else if(m_prp_view_mode->getStdString()=="FPS")
        {
            Ogre::Quaternion oqu_ogre_camera_rotation=Ogre::Quaternion(Ogre::Radian(-Ogre::Math::HALF_PI),Ogre::Vector3::UNIT_Y)*Ogre::Quaternion(Ogre::Radian(-Ogre::Math::HALF_PI),Ogre::Vector3::UNIT_Z);
            Ogre::Quaternion oqu_camera_corrected=camera_->getOrientation()*oqu_ogre_camera_rotation.Inverse();
            float f_yaw_new=oqu_camera_corrected.getRoll(false).valueRadians()-f_yaw;
            float f_pitch_new=oqu_camera_corrected.getYaw(false).valueRadians()+f_pitch;
            Ogre::Quaternion oqu_camera_pitch,oqu_camera_yaw;
            oqu_camera_yaw.FromAngleAxis(Ogre::Radian(f_yaw_new),Ogre::Vector3::UNIT_Z);
            oqu_camera_pitch.FromAngleAxis(Ogre::Radian(f_pitch_new),Ogre::Vector3::UNIT_Y);
            m_prp_view_focus->setVector(oqu_camera_yaw*oqu_camera_pitch*oqu_ogre_camera_rotation*Ogre::Vector3::NEGATIVE_UNIT_Z);
            // The m_prp_view_direction vector gets finally derived in the UpdateViewPose()
            // this updates the view properly!
        }
        else
        {
            // Do nothing for now...
        }
    }
    void RVizPluginViewAnimated::UpdateViewEye(float f_x,float f_y,float f_z)
    {
        Ogre::Vector3 ov3_translation(f_x,f_y,f_z);
        Ogre::Vector3 ov3_position_new=m_prp_view_eye->getVector()+camera_->getOrientation()*ov3_translation;
        if((ov3_position_new-m_prp_view_focus->getVector()).length()>m_prp_view_distance->getMin())
        {
            m_prp_view_eye->setVector(ov3_position_new);
        }
        m_prp_view_distance->setFloat(m_f_view_distance);
    }
    void RVizPluginViewAnimated::UpdateViewEyeFocus(float f_x,float f_y,float f_z)
    {
        Ogre::Vector3 ov3_translation(f_x,f_y,f_z);
        m_prp_view_eye->add(camera_->getOrientation()*ov3_translation);
        if(m_prp_view_mode->getStdString()=="ORBIT") m_prp_view_focus->add(camera_->getOrientation()*ov3_translation);
    }
    void RVizPluginViewAnimated::UpdateViewInitialize()
    {
        // Add current view as initial
        rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement_empty;
        msg_vmo_movement_empty.view_tf_frame=m_prp_view_tf_frame->getFrameStd();
        if(m_prp_view_mode->getStdString()=="ORBIT") msg_vmo_movement_empty.view_mode=0;
        if(m_prp_view_mode->getStdString()=="FPS") msg_vmo_movement_empty.view_mode=1;
        if(m_prp_view_speed_profile->getStdString()=="DECLINING") msg_vmo_movement_empty.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::DECLINING;
        if(m_prp_view_speed_profile->getStdString()=="RISING") msg_vmo_movement_empty.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::RISING;
        if(m_prp_view_speed_profile->getStdString()=="WAVE") msg_vmo_movement_empty.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::WAVE;
        if(m_prp_view_speed_profile->getStdString()=="FULL") msg_vmo_movement_empty.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::FULL;
        if(m_prp_view_speed_profile->getStdString()=="QUADRATIC") msg_vmo_movement_empty.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::QUADRATIC;
        if(m_prp_view_speed_profile->getStdString()=="EXPONENTIAL") msg_vmo_movement_empty.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::EXPONENTIAL;
        msg_vmo_movement_empty.view_duration.data=m_prp_view_duration->getFloat();
        msg_vmo_movement_empty.view_target_fps.data=m_prp_view_target_fps->getFloat();
        msg_vmo_movement_empty.view_frame_by_frame_enabled=m_prp_view_frame_by_frame_enabled->getBool();
        msg_vmo_movement_empty.view_vector_up_lock_enabled=m_prp_view_vector_up_lock_enabled->getBool();
        msg_vmo_movement_empty.view_pose_enabled=m_prp_view_pose_publish_enabled->getBool();
        msg_vmo_movement_empty.view_live_enabled=m_prp_view_live_publish_enabled->getBool();
        msg_vmo_movement_empty.view_mouse_enabled=m_prp_view_mouse_enabled->getBool();
        msg_vmo_movement_empty.view_eye.point.x=m_prp_view_eye->getVector().x;
        msg_vmo_movement_empty.view_eye.point.y=m_prp_view_eye->getVector().y;
        msg_vmo_movement_empty.view_eye.point.z=m_prp_view_eye->getVector().z;
        msg_vmo_movement_empty.view_focus.point.x=m_prp_view_focus->getVector().x;
        msg_vmo_movement_empty.view_focus.point.y=m_prp_view_focus->getVector().y;
        msg_vmo_movement_empty.view_focus.point.z=m_prp_view_focus->getVector().z;
        msg_vmo_movement_empty.view_up.vector.x=m_prp_view_up->getVector().x;
        msg_vmo_movement_empty.view_up.vector.y=m_prp_view_up->getVector().y;
        msg_vmo_movement_empty.view_up.vector.z=m_prp_view_up->getVector().z;
        m_vec_vmo_movements.push_back(msg_vmo_movement_empty);

        m_tim_view_animation_start=m_hdl_node->get_clock()->now();
        m_prp_view_animation_enabled->setBool(true);
    }
    void RVizPluginViewAnimated::UpdateViewCancel()
    {
        if(m_prp_view_animation_enabled->getBool()==true)
        {
            m_vec_vmo_movements.clear();
            m_f_frames_rendered=0.0;
            m_prp_view_animation_enabled->setBool(false);
            m_prp_view_frame_by_frame_enabled->setBool(false);
            PublishViewAnimationCompleted();
        }
    }
    float RVizPluginViewAnimated::CalculateViewAnimationIteration(float f_time_delta,uint8_t ui8_movement_mode)
    {
        // View movement speed profiles:
        // 0 (RISING) ... Increase speed through view trajectory.
        // 1 (DECLINING) ... Decrease speed through view trajectory.
        // 2 (WAVE) ... Wave-like speed-profile through view trajectory.
        // 3 (FULL) ... Constant speed through view trajectory.
        // 4 (QUADRATIC) ... Quadratically increasing speed through view trajectory.
        // 5 (EXPONENTIAL) ... Exponentially decreasing speed through view trajectory.
        switch(ui8_movement_mode)
        {
            case rviz_plugin_view_animated_msgs::msg::ViewMovement::DECLINING:
                return float(-cos(f_time_delta*M_PI_2+M_PI_2));
            break;

            case rviz_plugin_view_animated_msgs::msg::ViewMovement::RISING:
                return 1.0f-float(cos(f_time_delta*M_PI_2));
            break;

            case rviz_plugin_view_animated_msgs::msg::ViewMovement::WAVE:
                return 0.5f*(1.0f-float(cos(f_time_delta*M_PI)));
            break;

            case rviz_plugin_view_animated_msgs::msg::ViewMovement::FULL:
                return f_time_delta;
            break;

            case rviz_plugin_view_animated_msgs::msg::ViewMovement::QUADRATIC:
                return float(pow(f_time_delta,2.0));
            break;

            case rviz_plugin_view_animated_msgs::msg::ViewMovement::EXPONENTIAL:
                return float(1.0f-exp(-f_time_delta*m_prp_view_duration->getFloat()*2.0));
            break;

            default: // DECLINING
                return float(-cos(f_time_delta*M_PI_2+M_PI_2));
            break;
        }
    }
    void RVizPluginViewAnimated::OrbitViewTo(const Ogre::Vector3& ov3_view_eye)
    {
        UpdateViewInitialize();

        rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement;
        msg_vmo_movement.view_tf_frame=m_prp_view_tf_frame->getFrameStd();
        if(m_prp_view_mode->getStdString()=="ORBIT") msg_vmo_movement.view_mode=0;
        if(m_prp_view_mode->getStdString()=="FPS") msg_vmo_movement.view_mode=1;
        if(m_prp_view_speed_profile->getStdString()=="DECLINING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::DECLINING;
        if(m_prp_view_speed_profile->getStdString()=="RISING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::RISING;
        if(m_prp_view_speed_profile->getStdString()=="WAVE") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::WAVE;
        if(m_prp_view_speed_profile->getStdString()=="FULL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::FULL;
        if(m_prp_view_speed_profile->getStdString()=="QUADRATIC") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::QUADRATIC;
        if(m_prp_view_speed_profile->getStdString()=="EXPONENTIAL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::EXPONENTIAL;
        msg_vmo_movement.view_duration.data=m_prp_view_duration->getFloat();
        msg_vmo_movement.view_target_fps.data=m_prp_view_target_fps->getFloat();
        msg_vmo_movement.view_frame_by_frame_enabled=m_prp_view_frame_by_frame_enabled->getBool();
        msg_vmo_movement.view_vector_up_lock_enabled=m_prp_view_vector_up_lock_enabled->getBool();
        msg_vmo_movement.view_pose_enabled=m_prp_view_pose_publish_enabled->getBool();
        msg_vmo_movement.view_live_enabled=m_prp_view_live_publish_enabled->getBool();
        msg_vmo_movement.view_mouse_enabled=m_prp_view_mouse_enabled->getBool();
        msg_vmo_movement.view_eye.point.x=ov3_view_eye.x;
        msg_vmo_movement.view_eye.point.y=ov3_view_eye.y;
        msg_vmo_movement.view_eye.point.z=ov3_view_eye.z;
        msg_vmo_movement.view_focus.point.x=m_prp_view_focus->getVector().x;
        msg_vmo_movement.view_focus.point.y=m_prp_view_focus->getVector().y;
        msg_vmo_movement.view_focus.point.z=m_prp_view_focus->getVector().z;
        msg_vmo_movement.view_up.vector.x=m_prp_view_up->getVector().x;
        msg_vmo_movement.view_up.vector.y=m_prp_view_up->getVector().y;
        msg_vmo_movement.view_up.vector.z=m_prp_view_up->getVector().z;
        m_vec_vmo_movements.push_back(msg_vmo_movement);
    }
    void RVizPluginViewAnimated::MoveEyeWithFocusTo(const Ogre::Vector3& ov3_view_eye_focus)
    {
        UpdateViewInitialize();

        rviz_plugin_view_animated_msgs::msg::ViewMovement msg_vmo_movement;
        msg_vmo_movement.view_tf_frame=m_prp_view_tf_frame->getFrameStd();
        if(m_prp_view_mode->getStdString()=="ORBIT") msg_vmo_movement.view_mode=0;
        if(m_prp_view_mode->getStdString()=="FPS") msg_vmo_movement.view_mode=1;
        if(m_prp_view_speed_profile->getStdString()=="DECLINING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::DECLINING;
        if(m_prp_view_speed_profile->getStdString()=="RISING") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::RISING;
        if(m_prp_view_speed_profile->getStdString()=="WAVE") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::WAVE;
        if(m_prp_view_speed_profile->getStdString()=="FULL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::FULL;
        if(m_prp_view_speed_profile->getStdString()=="QUADRATIC") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::QUADRATIC;
        if(m_prp_view_speed_profile->getStdString()=="EXPONENTIAL") msg_vmo_movement.view_speed_profile=rviz_plugin_view_animated_msgs::msg::ViewMovement::EXPONENTIAL;
        msg_vmo_movement.view_duration.data=m_prp_view_duration->getFloat();
        msg_vmo_movement.view_target_fps.data=m_prp_view_target_fps->getFloat();
        msg_vmo_movement.view_frame_by_frame_enabled=m_prp_view_frame_by_frame_enabled->getBool();
        msg_vmo_movement.view_vector_up_lock_enabled=m_prp_view_vector_up_lock_enabled->getBool();
        msg_vmo_movement.view_pose_enabled=m_prp_view_pose_publish_enabled->getBool();
        msg_vmo_movement.view_live_enabled=m_prp_view_live_publish_enabled->getBool();
        msg_vmo_movement.view_mouse_enabled=m_prp_view_mouse_enabled->getBool();
        msg_vmo_movement.view_eye.point.x=ov3_view_eye_focus.x;
        msg_vmo_movement.view_eye.point.y=ov3_view_eye_focus.y;
        msg_vmo_movement.view_eye.point.z=ov3_view_eye_focus.z;
        msg_vmo_movement.view_focus.point.x=m_prp_view_focus->getVector().x+(ov3_view_eye_focus.x-m_prp_view_eye->getVector().x);
        msg_vmo_movement.view_focus.point.y=m_prp_view_focus->getVector().y+(ov3_view_eye_focus.y-m_prp_view_eye->getVector().y);
        msg_vmo_movement.view_focus.point.z=m_prp_view_focus->getVector().z+(ov3_view_eye_focus.z-m_prp_view_eye->getVector().z);
        msg_vmo_movement.view_up.vector.x=m_prp_view_up->getVector().x;
        msg_vmo_movement.view_up.vector.y=m_prp_view_up->getVector().y;
        msg_vmo_movement.view_up.vector.z=m_prp_view_up->getVector().z;
        m_vec_vmo_movements.push_back(msg_vmo_movement);
    }
    Ogre::Vector3 RVizPluginViewAnimated::FixedFrameToAttachedLocal(const Ogre::Vector3& ov3_vector)
    {
        return m_oqu_tf_frame_orientation.Inverse()*(ov3_vector-m_ov3_tf_frame_position);
    }
    Ogre::Vector3 RVizPluginViewAnimated::AttachedLocalToFixedFrame(const Ogre::Vector3& ov3_vector)
    {
        return m_ov3_tf_frame_position+(m_oqu_tf_frame_orientation*ov3_vector);
    }
    void RVizPluginViewAnimated::TransformViewTrajectoryToFixedTFFrame(geometry_msgs::msg::PointStamped& msg_pns_eye,
                                                             geometry_msgs::msg::PointStamped& msg_pns_focus,
                                                             geometry_msgs::msg::Vector3Stamped& msg_v3s_up,bool b_is_fps)
    {
        Ogre::Vector3 ov3_pos_eye_fixed,ov3_pos_focus_fixed,ov3_pos_up_fixed;
        Ogre::Quaternion oqu_ori_eye_fixed,oqu_ori_focus_fixed,oqu_ori_up_fixed;

        context_->getFrameManager()->getTransform(msg_pns_eye.header.frame_id,ov3_pos_eye_fixed,oqu_ori_eye_fixed);
        context_->getFrameManager()->getTransform(msg_pns_focus.header.frame_id,ov3_pos_focus_fixed,oqu_ori_focus_fixed);
        context_->getFrameManager()->getTransform(msg_v3s_up.header.frame_id,ov3_pos_up_fixed,oqu_ori_up_fixed);

        Ogre::Vector3 ov3_eye_ogre=Ogre::Vector3(msg_pns_eye.point.x,msg_pns_eye.point.y,msg_pns_eye.point.z);
        Ogre::Vector3 ov3_focus_ogre=Ogre::Vector3(msg_pns_focus.point.x,msg_pns_focus.point.y,msg_pns_focus.point.z);
        Ogre::Vector3 ov3_up_ogre=Ogre::Vector3(msg_v3s_up.vector.x,msg_v3s_up.vector.y,msg_v3s_up.vector.z);

        ov3_eye_ogre=ov3_pos_eye_fixed+oqu_ori_eye_fixed*ov3_eye_ogre;
        if(b_is_fps==true) ov3_focus_ogre=oqu_ori_focus_fixed*ov3_focus_ogre;
        else ov3_focus_ogre=ov3_pos_focus_fixed+oqu_ori_focus_fixed*ov3_focus_ogre;
        ov3_up_ogre=oqu_ori_up_fixed*ov3_up_ogre;

        msg_pns_eye.point.x=ov3_eye_ogre.x; msg_pns_eye.point.y=ov3_eye_ogre.y; msg_pns_eye.point.z=ov3_eye_ogre.z;
        msg_pns_focus.point.x=ov3_focus_ogre.x; msg_pns_focus.point.y=ov3_focus_ogre.y; msg_pns_focus.point.z=ov3_focus_ogre.z;
        msg_v3s_up.vector.x=ov3_up_ogre.x; msg_v3s_up.vector.y=ov3_up_ogre.y; msg_v3s_up.vector.z=ov3_up_ogre.z;
        msg_pns_eye.header.frame_id=m_prp_view_tf_frame->getStdString();
        msg_pns_focus.header.frame_id=m_prp_view_tf_frame->getStdString();
        msg_v3s_up.header.frame_id=m_prp_view_tf_frame->getStdString();
    }
    Ogre::Quaternion RVizPluginViewAnimated::getQuaternionFromDirection(const Ogre::Vector3& ov3_direction,const Ogre::Vector3& ov3_up_vector)
    {
        Ogre::Vector3 ov3_forward_vector=ov3_direction.normalisedCopy();
        Ogre::Vector3 ov3_right_vector=ov3_up_vector.crossProduct(ov3_forward_vector).normalisedCopy();
        Ogre::Vector3 ov3_up_vector_new=ov3_forward_vector.crossProduct(ov3_right_vector);
        Ogre::Matrix3 om3_orientation;
        om3_orientation.FromAxes(ov3_right_vector,ov3_up_vector_new,ov3_forward_vector);
        return Ogre::Quaternion(om3_orientation);
    }
    void RVizPluginViewAnimated::InitializeViewLiveTexture()
    {
        m_tep_view_live=Ogre::TextureManager::getSingleton().createManual(
                    "TextureViewLive"+std::to_string(si_resource_name_count),
                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    Ogre::TextureType::TEX_TYPE_2D,
                    m_prp_view_live_width->getInt(),
                    m_prp_view_live_height->getInt(),
                    0,
                    Ogre::PixelFormat::PF_R8G8B8,
                    Ogre::TextureUsage::TU_RENDERTARGET);
        m_p_ret_view_live=m_tep_view_live->getBuffer()->getRenderTarget();
        m_p_ret_view_live->addViewport(context_->getViewManager()->getCurrent()->getCamera());
        m_p_ret_view_live->getViewport(0)->setClearEveryFrame(true);
        m_p_ret_view_live->getViewport(0)->setBackgroundColour(m_prp_view_live_color->getOgreColor());
        m_p_ret_view_live->getViewport(0)->setOverlaysEnabled(false);
    }
    void RVizPluginViewAnimated::UpdateViewLiveImageResolution()
    {
        // Do nothing for now...
    }
    void RVizPluginViewAnimated::PublishViewPose()
    {
        if(m_prp_view_pose_publish_enabled->getBool())
        {
            // Publish pose message and TF2 stamped transform message
            m_msg_pst_camera.header.frame_id=context_->getFixedFrame().toStdString();
            m_msg_pst_camera.header.stamp=m_hdl_node->get_clock()->now();
            m_msg_pst_camera.pose.position.x=camera_->getPosition().x;
            m_msg_pst_camera.pose.position.y=camera_->getPosition().y;
            m_msg_pst_camera.pose.position.z=camera_->getPosition().z;
            tf2::Quaternion qua_rot_origin=tf2::Quaternion(camera_->getOrientation().x,camera_->getOrientation().y,camera_->getOrientation().z,camera_->getOrientation().w);
            tf2::Quaternion qua_rotation;
            qua_rotation.setRPY(0.0,1.570796327,1.570796327); // Rotate from OGRE camera frame into conventional x-forward frame
            tf2::Quaternion qua_rot_result=qua_rot_origin*qua_rotation;
            qua_rot_result.normalize();
            m_msg_pst_camera.pose.orientation.w=qua_rot_result.getW();
            m_msg_pst_camera.pose.orientation.x=qua_rot_result.getX();
            m_msg_pst_camera.pose.orientation.y=qua_rot_result.getY();
            m_msg_pst_camera.pose.orientation.z=qua_rot_result.getZ();
            m_pub_pst_view->publish(m_msg_pst_camera);

            m_tfs_transform.header.stamp=m_hdl_node->get_clock()->now();
            m_tfs_transform.header.frame_id=context_->getFixedFrame().toStdString();
            m_tfs_transform.child_frame_id="animated_"+std::to_string(si_resource_name_count);
            m_tfs_transform.transform.translation.x=camera_->getPosition().x;
            m_tfs_transform.transform.translation.y=camera_->getPosition().y;
            m_tfs_transform.transform.translation.z=camera_->getPosition().z;
            m_tfs_transform.transform.rotation.w=qua_rot_result.getW();
            m_tfs_transform.transform.rotation.x=qua_rot_result.getX();
            m_tfs_transform.transform.rotation.y=qua_rot_result.getY();
            m_tfs_transform.transform.rotation.z=qua_rot_result.getZ();
            tf2_broadcaster->sendTransform(m_tfs_transform);
        }
    }
    void RVizPluginViewAnimated::PublishViewAnimationCompleted()
    {
        std_msgs::msg::Bool msg_bol_animation_finished;
        msg_bol_animation_finished.data=1;
        m_pub_bol_view_animation_finished->publish(msg_bol_animation_finished);
    }
    void RVizPluginViewAnimated::PublishViewLiveImage()
    {
        if(m_prp_view_live_publish_enabled->getBool())
        {
            m_ui8_buffer.resize(m_prp_view_live_width->getInt()*m_prp_view_live_height->getInt()*3);
            m_p_ret_view_live->update();

            int i_img_width=m_prp_view_live_width->getInt();
            int i_img_height=m_prp_view_live_height->getInt();
            Ogre::PixelBox pb(i_img_width,i_img_height,1,Ogre::PF_BYTE_BGR,m_ui8_buffer.data());
            m_p_ret_view_live->copyContentsToMemory(pb,Ogre::RenderTarget::FB_AUTO);
            QImage qim_view_live(m_ui8_buffer.data(),i_img_width,i_img_height,i_img_width*3,QImage::Format_BGR888);

            // If enabled, record MJPEG video file:
            cv::Mat mat_view_live_frame(qim_view_live.height(),qim_view_live.width(),CV_8UC3,const_cast<uchar*>(qim_view_live.bits()),qim_view_live.bytesPerLine());
            if(m_prp_view_live_record_enabled->getBool()==true)
            {
                m_vwt_view_live.write(mat_view_live_frame);
            }

            m_msg_img_view_live.header.frame_id=m_prp_view_tf_frame->getStdString();
            m_msg_img_view_live.header.stamp=m_hdl_node->get_clock()->now();
            m_msg_img_view_live.format="jpeg";
            cv::imencode(".jpg",mat_view_live_frame,m_msg_img_view_live.data,m_vec_view_live_image_params);

            m_pub_img_view_live->publish(m_msg_img_view_live);
        }
    }

    // Default constructor and destructor
    RVizPluginViewAnimated::RVizPluginViewAnimated()
    {
        // Do nothing for now...
    }
    RVizPluginViewAnimated::~RVizPluginViewAnimated()
    {
        m_p_ret_view_live->setAutoUpdated(false);
        m_p_ret_view_live->setActive(false);
        Ogre::TextureManager::getSingleton().remove(m_tep_view_live->getName());
        m_tep_view_live->freeInternalResources();
    }

}  // namespace rviz_plugin_view_animated

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_view_animated::RVizPluginViewAnimated,rviz_common::ViewController)
