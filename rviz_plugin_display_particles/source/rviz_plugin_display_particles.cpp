#include<rviz_plugin_display_particles.hpp>



namespace rviz_plugin_display_particles
{
    // Inherited methods
    void RVizPluginDisplayParticles::onInitialize()
    {
        // Initialize ROS2 properties
        m_hdl_node_rviz=context_->getRosNodeAbstraction().lock();
        m_hdl_node=m_hdl_node_rviz->get_raw_node();

        // Initialize helper properties
        si_resource_name_count++;
        m_s_rviz_plugin_namespace="rviz_plugin_display_particles_"+std::to_string(si_resource_name_count)+"/";

        // Initialize RViz properties
        m_prp_particles_tf_frame=std::make_unique<rviz_common::properties::TfFrameProperty>("TF Frame",rviz_common::properties::TfFrameProperty::FIXED_FRAME_STRING,"TF Frame.",this,(rviz_common::FrameManager*)context_->getFrameManager(),true,SLOT(cb_prp_update_particles_tf_frame()));
        m_prp_particles_position=std::make_unique<rviz_common::properties::VectorProperty>("Position",Ogre::Vector3::ZERO,"Position.",this,SLOT(cb_prp_update_particles_position()));
        m_prp_particles_direction=std::make_unique<rviz_common::properties::VectorProperty>("Direction",Ogre::Vector3(0.0,0.0,1.0),"Direction.",this,SLOT(cb_prp_update_particles_direction()));
        m_prp_particles_preset=std::make_unique<rviz_common::properties::EnumProperty>("Preset","Water Leak","Preset.",this,SLOT(cb_prp_update_particles_preset()));
        m_prp_particles_preset->addOption("Water Leak",0);
        m_prp_particles_preset->addOption("Gas Leak",1);
        m_prp_particles_preset->addOption("Smoke",2);
        m_prp_particles_preset->addOption("Fire",3);
        m_prp_particles_type=std::make_unique<rviz_common::properties::EnumProperty>("Type","Point","Type.",this,SLOT(cb_prp_update_particles_type()));
        m_prp_particles_type->addOption("Point",0);
        /* TODO: Add more particle emitters with all kind of particle types here!
        m_prp_particles_type->addOption("Area",1);
        m_prp_particles_type->addOption("Box",2);
        m_prp_particles_type->addOption("Cylinder",3);
        m_prp_particles_type->addOption("Ellipsoid",4);
        m_prp_particles_type->addOption("HollowEllipsoid",5);
        m_prp_particles_type->addOption("Ring",6);
        */
        m_prp_particles_quota=std::make_unique<rviz_common::properties::IntProperty>("Quota",1000,"Quota.",this,SLOT(cb_prp_update_particles_quota()));
        m_prp_particles_emission_rate=std::make_unique<rviz_common::properties::IntProperty>("Emission Rate",200,"Emission Rate.",this,SLOT(cb_prp_update_particles_emission_rate()));
        m_prp_particles_angle=std::make_unique<rviz_common::properties::FloatProperty>("Angle",0.5f,"Angle.",this,SLOT(cb_prp_update_particles_angle()));
        m_prp_particles_size=std::make_unique<rviz_common::properties::FloatProperty>("Size",0.01f,"Size.",this,SLOT(cb_prp_update_particles_size()));
        m_prp_particles_color_range_start=std::make_unique<rviz_common::properties::ColorProperty>("Color Range Start",QColor(0,0,0,0),"Color Range Start.",this,SLOT(cb_prp_update_particles_color()));
        m_prp_particles_color_range_end=std::make_unique<rviz_common::properties::ColorProperty>("Color Range End",QColor(255,255,255,255),"Color Range End.",this,SLOT(cb_prp_update_particles_color()));
        m_prp_particles_time_to_live=std::make_unique<rviz_common::properties::FloatProperty>("Time To Live",5.0f,"Time To Live.",this,SLOT(cb_prp_update_particles_time_to_live()));
        m_prp_particles_velocity_max=std::make_unique<rviz_common::properties::FloatProperty>("Velocity Max",0.25f,"Velocity Max.",this,SLOT(cb_prp_update_particles_velocity_max()));
        m_prp_particles_colour_fader=std::make_unique<rviz_common::properties::VectorProperty>("Fader Colour",Ogre::Vector3::ZERO,"Fader Colour.",this,SLOT(cb_prp_update_particles_colour_fader()));
        m_prp_particles_colour_fader_alpha=std::make_unique<rviz_common::properties::FloatProperty>("Fader Alpha",0.0f,"Fader Alpha.",this,SLOT(cb_prp_update_particles_colour_fader()));
        m_prp_particles_linear_force=std::make_unique<rviz_common::properties::VectorProperty>("Linear Force",Ogre::Vector3(0.0,0.5,0.0),"Linear Force.",this,SLOT(cb_prp_update_particles_linear_force()));
        m_prp_particles_scaler=std::make_unique<rviz_common::properties::FloatProperty>("Scaler",0.0f,"Scaler.",this,SLOT(cb_prp_update_particles_scaler()));
        m_prp_particles_randomness=std::make_unique<rviz_common::properties::IntProperty>("Randomness",3,"Randomness.",this,SLOT(cb_prp_update_particles_randomness()));
        m_prp_particles_scope=std::make_unique<rviz_common::properties::FloatProperty>("Scope",0.5f,"Scope.",this,SLOT(cb_prp_update_particles_randomness()));
        m_prp_particles_keep_velocity=std::make_unique<rviz_common::properties::BoolProperty>("Keep Velocity",false,"Keep Velocity.",this,SLOT(cb_prp_update_particles_randomness()));

        // Initialize OGRE properties
        UpdateParticlesOgreResources();
        //UpdateParticlesSandbox();

        setStatusStd(rviz_common::properties::StatusProperty::Ok,"Particles","Ok!");
        RVIZ_COMMON_LOG_INFO_STREAM("rviz_plugin_display_particles::RVizPluginDisplayParticles - Initialized!");
    }
    void RVizPluginDisplayParticles::onDisable()
    {
        m_ogr_particlesystem->setEmitting(false);
    }
    void RVizPluginDisplayParticles::onEnable()
    {
        m_ogr_particlesystem->setEmitting(true);
    }
    void RVizPluginDisplayParticles::reset()
    {
        // Do nothing for now...
    }
    void RVizPluginDisplayParticles::update(float wall_dt,float ros_dt)
    {
        // Do nothing for now...
    }

    // Callbacks to process ROS2 messages
    // ...

    // Callbacks to process property updates
    void RVizPluginDisplayParticles::cb_prp_update_particles_tf_frame()
    {
        if(context_->getFrameManager()->getTransform(m_prp_particles_tf_frame->getFrameStd(),m_ov3_tf_frame_position,m_oqu_tf_frame_orientation))
        {
            UpdateParticlesPosition();
            UpdateParticlesDirection();
            setStatusStd(rviz_common::properties::StatusProperty::Ok,"Particles","Ok!");
        }
        else
        {
            setStatusStd(rviz_common::properties::StatusProperty::Warn,"Particles","No valid transform!");
        }
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_position()
    {
        UpdateParticlesPosition();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_direction()
    {
        UpdateParticlesDirection();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_preset()
    {
        UpdateParticlesPreset();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_type()
    {
        UpdateParticlesType();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_quota()
    {
        UpdateParticlesQuota();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_emission_rate()
    {
        UpdateParticlesEmissionRate();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_angle()
    {
        UpdateParticlesAngle();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_size()
    {
        UpdateParticlesSize();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_color()
    {
        UpdateParticlesColor();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_time_to_live()
    {
        UpdateParticlesTimeToLive();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_velocity_max()
    {
        UpdateParticlesVelocityMax();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_colour_fader()
    {
        UpdateParticlesColourFader();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_linear_force()
    {
        UpdateParticlesLinearForce();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_scaler()
    {
        UpdateParticlesScaler();
    }
    void RVizPluginDisplayParticles::cb_prp_update_particles_randomness()
    {
        UpdateParticlesRandomness();
    }

    // Helper methods
    void RVizPluginDisplayParticles::UpdateParticlesOgreResources()
    {
        Ogre::Root::getSingleton().loadPlugin("/opt/ros/jazzy/opt/rviz_ogre_vendor/lib/OGRE/Plugin_ParticleFX.so");

        m_prp_particles_tf_frame->setFrameManager(context_->getFrameManager());
        m_ov3_tf_frame_position=Ogre::Vector3(0.0,0.0,0.0);
        m_oqu_tf_frame_orientation=Ogre::Quaternion(1.0,0.0,0.0,0.0);

        Ogre::MaterialPtr material=Ogre::MaterialManager::getSingleton().create("FlatVertexColor"+std::to_string(si_resource_name_count),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        material->setCullingMode(Ogre::CULL_NONE);
        material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
        material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        material->getTechnique(0)->setLightingEnabled(false); // Necessary to allow for vertex color!

        m_ogr_particlesystem=context_->getSceneManager()->createParticleSystem();
        context_->getSceneManager()->getRootSceneNode()->attachObject(m_ogr_particlesystem);
        m_ogr_particlesystem->setDefaultDimensions(0.01,0.01);
        m_ogr_particlesystem->setSortingEnabled(true);
        m_ogr_particlesystem->setCullIndividually(true);
        m_ogr_particlesystem->setParticleQuota(1000);
        m_ogr_particlesystem->setRenderer("billboard");
        m_ogr_particlesystem->setKeepParticlesInLocalSpace(false);
        m_ogr_particlesystem->setDebugDisplayEnabled(false);
        m_ogr_particlesystem->setVisible(true);
        m_ogr_particlesystem->setMaterialName("FlatVertexColor"+std::to_string(si_resource_name_count));
        m_ogr_particlesystem->clear();

        m_ogr_particlesystem->addAffector("ColourFader");
        m_ogr_particlesystem->getAffector(0)->setParameter("red","0.0");
        m_ogr_particlesystem->getAffector(0)->setParameter("green","0.0");
        m_ogr_particlesystem->getAffector(0)->setParameter("blue","0.0");
        m_ogr_particlesystem->getAffector(0)->setParameter("alpha","0.0");
        m_ogr_particlesystem->addAffector("LinearForce");
        m_ogr_particlesystem->getAffector(1)->setParameter("force_vector","0 0.5 0");
        m_ogr_particlesystem->addAffector("Scaler");
        m_ogr_particlesystem->getAffector(2)->setParameter("rate","0.0");
        m_ogr_particlesystem->addAffector("DirectionRandomiser");
        m_ogr_particlesystem->getAffector(3)->setParameter("randomness","3");
        m_ogr_particlesystem->getAffector(3)->setParameter("scope","0.5");
        m_ogr_particlesystem->getAffector(3)->setParameter("keep_velocity","false");

        m_ogr_particlesystem->addEmitter("Point");
        m_ogr_particlesystem->getEmitter(0)->setPosition(Ogre::Vector3(0.0,0.0,0.0));
        m_ogr_particlesystem->getEmitter(0)->setDirection(Ogre::Vector3(0.0,0.0,1.0));
        m_ogr_particlesystem->getEmitter(0)->setColour(Ogre::ColourValue(0.0,0.0,0.0,1.0));
        m_ogr_particlesystem->getEmitter(0)->setColourRangeStart(Ogre::ColourValue(0.0,0.0,0.0,1.0));
        m_ogr_particlesystem->getEmitter(0)->setColourRangeEnd(Ogre::ColourValue(0.0,0.0,0.0,1.0));
        m_ogr_particlesystem->getEmitter(0)->setAngle(Ogre::Radian(0.5));
        m_ogr_particlesystem->getEmitter(0)->setEmissionRate(200);
        m_ogr_particlesystem->getEmitter(0)->setMaxParticleVelocity(0.25);
        m_ogr_particlesystem->getEmitter(0)->setTimeToLive(5.0);
        m_ogr_particlesystem->getEmitter(0)->setParameter("colour","1 1 1 1");
        m_ogr_particlesystem->getEmitter(0)->setParameter("colour_range_start","1 1 1 1");
        m_ogr_particlesystem->getEmitter(0)->setParameter("colour_range_end","1 1 1 1");
        m_ogr_particlesystem->getEmitter(0)->setEnabled(true);

        // TODO: Add more particle emitters with all kind of particle types here!
        m_ogr_particlesystem->addEmitter("Ellipsoid");
        m_ogr_particlesystem->getEmitter(1)->setParameter("width","0.5");
        m_ogr_particlesystem->getEmitter(1)->setParameter("height","0.5");
        m_ogr_particlesystem->getEmitter(1)->setParameter("depth","0.5");
        m_ogr_particlesystem->getEmitter(1)->setParameter("angle","30");
        m_ogr_particlesystem->getEmitter(1)->setParameter("emission_rate","200");
        m_ogr_particlesystem->getEmitter(1)->setParameter("time_to_live_min","2");
        m_ogr_particlesystem->getEmitter(1)->setParameter("time_to_live_max","5");
        m_ogr_particlesystem->getEmitter(1)->setParameter("direction","0 0 1");
        m_ogr_particlesystem->getEmitter(1)->setParameter("velocity","0.1");
        m_ogr_particlesystem->getEmitter(1)->setParameter("colour","0.15 0.1 0.0");
        m_ogr_particlesystem->getEmitter(1)->setEnabled(false);

        UpdateParticlesPreset();
    }
    void RVizPluginDisplayParticles::UpdateParticlesSandbox()
    {
        // Load plugin
        Ogre::Root::getSingleton().loadPlugin("/opt/ros/jazzy/opt/rviz_ogre_vendor/lib/OGRE/Plugin_ParticleFX.so");

        // Create particle system
        Ogre::ParticleSystem* smoke=context_->getSceneManager()->createParticleSystem("SmokeSystem", 500);

        // Set billboard material
        smoke->setMaterialName("Examples/Smoke");
        smoke->setDefaultDimensions(0.1f,0.1f);

        // Emitter
        Ogre::ParticleEmitter* emitter=smoke->addEmitter("Point");
        emitter->setEmissionRate(40); // particles/sec
        emitter->setParticleVelocity(15.0f);
        emitter->setDirection(Ogre::Vector3::UNIT_Y);
        emitter->setAngle(Ogre::Degree(20));
        emitter->setTimeToLive(4.0f);
        emitter->setColour(Ogre::ColourValue(0.3f, 0.3f, 0.3f, 0.8f));
        emitter->setDuration(0); // infinite
        emitter->setRepeatDelay(0);

        // Affector
        Ogre::ParticleAffector* colourFade=smoke->addAffector("ColourFader");
        colourFade->setParameter("red",   "-0.05");
        colourFade->setParameter("green", "-0.05");
        colourFade->setParameter("blue",  "-0.05");
        colourFade->setParameter("alpha", "-0.25");

        // Drift
        Ogre::ParticleAffector* linearForce=smoke->addAffector("LinearForce");
        linearForce->setParameter("force_vector", "0 3 0");
        linearForce->setParameter("force_application", "add");

        // Attach to scene
        Ogre::SceneNode* smokeNode=context_->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        smokeNode->attachObject(smoke);
        smoke->setEmitting(true);
    }
    void RVizPluginDisplayParticles::UpdateParticlesPosition()
    {
        m_ogr_particlesystem->getEmitter(0)->setPosition(m_ov3_tf_frame_position+m_prp_particles_position->getVector());
    }
    void RVizPluginDisplayParticles::UpdateParticlesDirection()
    {
        m_ogr_particlesystem->getEmitter(0)->setDirection(m_oqu_tf_frame_orientation*m_prp_particles_direction->getVector());
    }
    void RVizPluginDisplayParticles::UpdateParticlesPreset()
    {
        switch(m_prp_particles_preset->getOptionInt())
        {
            case 0: // WATER LEAK
                m_prp_particles_quota->setInt(2000);
                m_prp_particles_emission_rate->setInt(1000);
                m_prp_particles_angle->setFloat(0.125);
                m_prp_particles_size->setFloat(0.025);
                m_prp_particles_color_range_start->setColor(QColor(32,74,135));
                m_prp_particles_color_range_end->setColor(QColor(32,74,135));
                m_prp_particles_time_to_live->setFloat(2.0);
                m_prp_particles_velocity_max->setFloat(0.25);
                m_prp_particles_colour_fader->setVector(Ogre::Vector3(0.2,0.2,0.2));
                m_prp_particles_colour_fader_alpha->setFloat(-0.25);
                m_prp_particles_linear_force->setVector(Ogre::Vector3(0.0,0.0,0.0));
                m_prp_particles_scaler->setFloat(-0.005);
                m_prp_particles_randomness->setInt(1);
                m_prp_particles_scope->setFloat(0.25);
                m_prp_particles_keep_velocity->setBool(true);
                UpdateParticlesPropertiesAll();
            break;

            case 1: // GAS LEAK
                m_prp_particles_quota->setInt(2000);
                m_prp_particles_emission_rate->setInt(1000);
                m_prp_particles_angle->setFloat(0.125);
                m_prp_particles_size->setFloat(0.05);
                m_prp_particles_color_range_start->setColor(QColor(255,255,255));
                m_prp_particles_color_range_end->setColor(QColor(255,255,255));
                m_prp_particles_time_to_live->setFloat(2.0);
                m_prp_particles_velocity_max->setFloat(0.25);
                m_prp_particles_colour_fader->setVector(Ogre::Vector3(-0.1,-0.1,-0.1));
                m_prp_particles_colour_fader_alpha->setFloat(-0.25);
                m_prp_particles_linear_force->setVector(Ogre::Vector3(0.0,0.3,0.0));
                m_prp_particles_scaler->setFloat(-0.0075);
                m_prp_particles_randomness->setInt(3);
                m_prp_particles_scope->setFloat(0.75);
                m_prp_particles_keep_velocity->setBool(false);
                UpdateParticlesPropertiesAll();
            break;

            case 2: // SMOKE
                m_prp_particles_quota->setInt(2000);
                m_prp_particles_emission_rate->setInt(500);
                m_prp_particles_angle->setFloat(0.25);
                m_prp_particles_size->setFloat(0.04);
                m_prp_particles_color_range_start->setColor(QColor(128,128,128));
                m_prp_particles_color_range_end->setColor(QColor(100,100,100));
                m_prp_particles_time_to_live->setFloat(4.0);
                m_prp_particles_velocity_max->setFloat(0.25);
                m_prp_particles_colour_fader->setVector(Ogre::Vector3(-0.05,-0.05,-0.05));
                m_prp_particles_colour_fader_alpha->setFloat(-0.2);
                m_prp_particles_linear_force->setVector(Ogre::Vector3(0.0,0.5,0.0));
                m_prp_particles_scaler->setFloat(-0.01);
                m_prp_particles_randomness->setInt(2);
                m_prp_particles_scope->setFloat(0.5);
                m_prp_particles_keep_velocity->setBool(false);
                UpdateParticlesPropertiesAll();
            break;

            case 3: // FIRE
                m_prp_particles_quota->setInt(2000);
                m_prp_particles_emission_rate->setInt(500);
                m_prp_particles_angle->setFloat(0.25);
                m_prp_particles_size->setFloat(0.04);
                m_prp_particles_color_range_start->setColor(QColor(252,233,79));
                m_prp_particles_color_range_end->setColor(QColor(245,121,0));
                m_prp_particles_time_to_live->setFloat(4.0);
                m_prp_particles_velocity_max->setFloat(0.25);
                m_prp_particles_colour_fader->setVector(Ogre::Vector3(-0.5,-0.5,-0.5));
                m_prp_particles_colour_fader_alpha->setFloat(-0.2);
                m_prp_particles_linear_force->setVector(Ogre::Vector3(0.0,0.5,0.0));
                m_prp_particles_scaler->setFloat(-0.01);
                m_prp_particles_randomness->setInt(2);
                m_prp_particles_scope->setFloat(0.5);
                m_prp_particles_keep_velocity->setBool(false);
                UpdateParticlesPropertiesAll();
            break;

            default:
            break;
        }
    }
    void RVizPluginDisplayParticles::UpdateParticlesType()
    {
        switch(m_prp_particles_type->getOptionInt())
        {
            case 0: // POINT
                m_ogr_particlesystem->getEmitter(0)->setEnabled(true);
                m_ogr_particlesystem->getEmitter(1)->setEnabled(false);
            break;

            case 4: // ELLIPSOID
                m_ogr_particlesystem->getEmitter(0)->setEnabled(false);
                m_ogr_particlesystem->getEmitter(1)->setEnabled(true);
            break;
        }
    }
    void RVizPluginDisplayParticles::UpdateParticlesQuota()
    {
        m_ogr_particlesystem->setParticleQuota(m_prp_particles_quota->getInt());
    }
    void RVizPluginDisplayParticles::UpdateParticlesEmissionRate()
    {
        m_ogr_particlesystem->getEmitter(0)->setEmissionRate(m_prp_particles_emission_rate->getInt());
    }
    void RVizPluginDisplayParticles::UpdateParticlesAngle()
    {
        m_ogr_particlesystem->getEmitter(0)->setAngle(Ogre::Radian(m_prp_particles_angle->getFloat()));
    }
    void RVizPluginDisplayParticles::UpdateParticlesSize()
    {
        m_ogr_particlesystem->setDefaultDimensions(m_prp_particles_size->getFloat(),m_prp_particles_size->getFloat());
    }
    void RVizPluginDisplayParticles::UpdateParticlesColor()
    {
        m_ogr_particlesystem->getEmitter(0)->setColourRangeStart(m_prp_particles_color_range_start->getOgreColor());
        m_ogr_particlesystem->getEmitter(0)->setColourRangeEnd(m_prp_particles_color_range_end->getOgreColor());
    }
    void RVizPluginDisplayParticles::UpdateParticlesTimeToLive()
    {
        m_ogr_particlesystem->getEmitter(0)->setTimeToLive(m_prp_particles_time_to_live->getFloat());
    }
    void RVizPluginDisplayParticles::UpdateParticlesVelocityMax()
    {
        m_ogr_particlesystem->getEmitter(0)->setMaxParticleVelocity(m_prp_particles_velocity_max->getFloat());
    }
    void RVizPluginDisplayParticles::UpdateParticlesColourFader()
    {
        m_ogr_particlesystem->getAffector(0)->setParameter("red",std::to_string(m_prp_particles_colour_fader->getVector().x));
        m_ogr_particlesystem->getAffector(0)->setParameter("green",std::to_string(m_prp_particles_colour_fader->getVector().y));
        m_ogr_particlesystem->getAffector(0)->setParameter("blue",std::to_string(m_prp_particles_colour_fader->getVector().z));
        m_ogr_particlesystem->getAffector(0)->setParameter("alpha",std::to_string(m_prp_particles_colour_fader_alpha->getFloat()));
    }
    void RVizPluginDisplayParticles::UpdateParticlesLinearForce()
    {
        m_ogr_particlesystem->getAffector(1)->setParameter("force_vector",
                                                            std::to_string(m_prp_particles_linear_force->getVector().x)+" "+
                                                            std::to_string(m_prp_particles_linear_force->getVector().y)+" "+
                                                            std::to_string(m_prp_particles_linear_force->getVector().z) );
    }
    void RVizPluginDisplayParticles::UpdateParticlesScaler()
    {
        m_ogr_particlesystem->getAffector(2)->setParameter("rate",std::to_string(m_prp_particles_scaler->getFloat()));
    }
    void RVizPluginDisplayParticles::UpdateParticlesRandomness()
    {
        m_ogr_particlesystem->getAffector(3)->setParameter("randomness",std::to_string(m_prp_particles_randomness->getInt()));
        m_ogr_particlesystem->getAffector(3)->setParameter("scope",std::to_string(m_prp_particles_scope->getFloat()));
        m_ogr_particlesystem->getAffector(3)->setParameter("keep_velocity",std::to_string(m_prp_particles_keep_velocity->getBool()));
    }
    void RVizPluginDisplayParticles::UpdateParticlesPropertiesAll()
    {
        m_ogr_particlesystem->setParticleQuota(m_prp_particles_quota->getInt());
        m_ogr_particlesystem->getEmitter(0)->setEmissionRate(m_prp_particles_emission_rate->getInt());
        m_ogr_particlesystem->getEmitter(0)->setAngle(Ogre::Radian(m_prp_particles_angle->getFloat()));
        m_ogr_particlesystem->setDefaultDimensions(m_prp_particles_size->getFloat(),m_prp_particles_size->getFloat());
        m_ogr_particlesystem->getEmitter(0)->setColourRangeStart(m_prp_particles_color_range_start->getOgreColor());
        m_ogr_particlesystem->getEmitter(0)->setColourRangeEnd(m_prp_particles_color_range_end->getOgreColor());
        m_ogr_particlesystem->getEmitter(0)->setTimeToLive(m_prp_particles_time_to_live->getFloat());
        m_ogr_particlesystem->getEmitter(0)->setMaxParticleVelocity(m_prp_particles_velocity_max->getFloat());
        m_ogr_particlesystem->getAffector(0)->setParameter("red",std::to_string(m_prp_particles_colour_fader->getVector().x));
        m_ogr_particlesystem->getAffector(0)->setParameter("green",std::to_string(m_prp_particles_colour_fader->getVector().y));
        m_ogr_particlesystem->getAffector(0)->setParameter("blue",std::to_string(m_prp_particles_colour_fader->getVector().z));
        m_ogr_particlesystem->getAffector(0)->setParameter("alpha",std::to_string(m_prp_particles_colour_fader_alpha->getFloat()));
        m_ogr_particlesystem->getAffector(1)->setParameter("force_vector",
                                                            std::to_string(m_prp_particles_linear_force->getVector().x)+" "+
                                                            std::to_string(m_prp_particles_linear_force->getVector().y)+" "+
                                                            std::to_string(m_prp_particles_linear_force->getVector().z) );
        m_ogr_particlesystem->getAffector(2)->setParameter("rate",std::to_string(m_prp_particles_scaler->getFloat()));
        m_ogr_particlesystem->getAffector(3)->setParameter("randomness",std::to_string(m_prp_particles_randomness->getInt()));
        m_ogr_particlesystem->getAffector(3)->setParameter("scope",std::to_string(m_prp_particles_scope->getFloat()));
        m_ogr_particlesystem->getAffector(3)->setParameter("keep_velocity",std::to_string(m_prp_particles_keep_velocity->getBool()));
    }

    // Default constructor and destructor
    RVizPluginDisplayParticles::RVizPluginDisplayParticles()
    {
        // Do nothing for now...
    }
    RVizPluginDisplayParticles::~RVizPluginDisplayParticles()
    {
        // Do nothing for now...
    }

}  // namespace rviz_plugin_display_particles

#include<pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_display_particles::RVizPluginDisplayParticles,rviz_common::Display)
