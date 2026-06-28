# ias_rviz_plugins
A collection of RViz plugins for the InterActiveSpace (IAS).
Please keep in mind, that this package contains open source and untested software, provided "as is".
The package is currently implemented for **ROS2 Jazzy Jalisco**.



## 1. rviz_plugin_display_lighting - An RViz Display plugin to enable lighting and shadows.
This plugin extends RViz with different light sources and shadow mapping.

![Image not available!](rviz_plugin_display_lighting.png?raw=true "rviz_plugin_display_lighting")

The plugin is an extended version of the classic [**rviz_lighting**](https://github.com/mogumbo/rviz_lighting) ROS1 plugin, authored and/or maintained by Terence Welsh.
**Known issues**: Please note that shadow mapping is currently not supported, potentially due to an [**issue with the ogre_vendor library**](https://github.com/gazebosim/gazebo-classic/issues/2700). Please keep in mind, that this repository contains open source and untested software, provided "as is". It is work in progress as part of the InterActiveSpace (IAS).

### 1.1 Basic use of the plugin
After adding the plugin, the default light of RViz gets disabled and the plugin takes control of the "Ambient light".
Additionally, depending on what type of light you choose ("Point light", "Spot light", and "Sun light"), it affects Diffuse color and Specular color of the scene.
Experimenting with the available properties should result in the desired lighting effect.
Once enabled, a marker gets published for each type of light to facilitate understanding of the lighting effects.
You should set the correct marker path, so that the markers of the different light sources get visualized correctly.
Be aware that improper mesh geometry of the environment (e.g., multiple triangles on an actually flat surface) may result in distorted lighting.

### 1.2 Properties of the plugin
The following properties are currently used by the plugin: 
- **TF Frame** ... The local coordinate frame of the lighting pose.
- **Type** ... The lighting type ("Ambient light" only, "Point light", "Spot light", or "Sun light", that is a time-dependent "Point light"). 
- **Shadows Enabled** ... Enables or disables shadow mapping (currently not supported!).
- **Marker Enabled** ... Enables or disables publishing of a visual marker for each type of lighting.
- **Ambient Color** ... The color of the Ambient lighting effect for the RViz scene.
- **Diffuse Color** ... The color of the Diffuse lighting effect for the RViz scene.
- **Specular Color** ... The color of the Specular lighting effect for the RViz scene.
- **Diffuse/Specular HSV** ... Enables updating the Diffuse Color and Specular Color by using a HSV color space. 
- **Diffuse/Specular YCbCr** ... Enables updating the Diffuse Color and Specular Color by using a YCbCr color space.
- **Position** ... The relative position of the light expressed in the local coordinate frame.
- **Orientation** ... The relative orientation of the light expressed in the local coordinate frame.
- **Direction** ... The direction of the light expressed in the local coordinate frame.
- **Range** ... The maximum range of the selected lighting.
- **Attenuation** ... The attenuation of the selected lighting, summarized in an Ogre::Vector3(constant,linear,quadratic).
- **Inner Cone Angle** ... The inner cone angle in radians, relevant for the Spot light (currently not supported by OpenGL!).
- **Outer Cone Angle** ... The outer cone angle in radians, relevant for the Spot light.
- **Falloff Rate** ... The falloff rate of the lighting.
- **Sun Distance** ... The radial distance of the Sun light in relation to the full pose of the lighting. The Sun light rises from the (-y) direction or East and lowers towards (+y) direction or West.
- **Sun Rise Hour** ... The sun rise hour provided as integer. At sunrise hour the Sun light is aligned with the -y axis.
- **Marker Path** ... The resource path to the marker meshes for the "Point light", "Spot light", or "Sun light".
- **Marker Topic** ... The ROS2 topic where the marker is published.
- **Color Ambient Topic** ... The ROS2 topic where incoming messages may overwrite Ambient color settings. 
- **Diffuse/Specular Topic** ... The ROS2 topic where incoming messages may overwrite Diffuse/Specular color settings.
- **Enable Topic** ... The ROS2 topic where incoming messages switch on/off the lighting.

### 1.3 References (MLA):
- Saravanan, Gc, Govindarajan Yamuna, and S. Nandhini. "Real time implementation of RGB to HSV/HSI/HSL and its reverse color space models." 2016 International conference on communication and signal processing (ICCSP). IEEE, 2016.



## 2. rviz_plugin_display_particles - An RViz Display plugin to create particle effects.
This plugin extends RViz with different particle effects.

![Image not available!](rviz_plugin_display_particles.png?raw=true "rviz_plugin_display_particles")

### 2.1 Basic use of the plugin
After adding the plugin, you can select Presets (e.g., water leak, gas leak, smoke, fire, etc.).
Experimenting with the available properties should result in the desired particle effect.
Be sure to select the proper TF Frame for the particle emitter.

### 2.2 Properties of the plugin
The following properties are currently used by the plugin:
- **TF Frame** ... The local coordinate frame of the particle emitter.
- **Position** ... The position of the particle emitter in the local TF Frame.
- **Direction** ... The direction of the particle emitter in the local TF Frame.
- **Preset** ... The presets to choose from (e.g., water leak, gas leak, smoke, fire).
- **Type** ... The local coordinate frame of the lighting pose.
- **Quota** ... The local coordinate frame of the lighting pose.
- **Emission Rate** ... The local coordinate frame of the lighting pose.
- **Angle** ... The angular range in where the particles are emitted.
- **Size** ... The size of the emitted particles.
- **Color Range Start/End** ... The color range of the emitted particles.
- **Time To Live** ... The time after the emitted particles disappear.
- **Velocity Max** ... The maximum movement velocity of the emitted particles.
- **Fader Colour** ... The amount of fading for each color, provided in the RGB space.
- **Fader Alpha** ... The amount of fading for the transparency.
- **Linear Force** ... The linear force that affects the emitted particles (e.g., to produce wind effects).
- **Scaler** ... The amount of change in size of the emitted particles.
- **Randomness** ... The randomness to apply to the emitted particles.
- **Scope** ... The percentage of emitted particles which are randomized.
- **Keep Velocity** ... The particles either keep constant velocity or not.

### 2.3 References (MLA):
- ...



## 3. rviz_plugin_display_projector - An RViz Display plugin to project an image message to a scalable planar texture in 3D.
This plugin takes an image message and projects it onto a scalable flat texture positioned in 3D.

![Image not available!](rviz_plugin_display_projector.png?raw=true "rviz_plugin_display_projector")

The plugin is an extended version of the classic [**rviz_textured_quads**](https://github.com/lucasw/rviz_textured_quads) ROS1 plugin, authored and/or maintained by Felipe Bacim, Mohit Shridhar, and Lucas Walter.

### 3.1 Basic use of the plugin
After adding the plugin, you want to select the proper image topic to subscribe to. Adapt the other properties as necessary.

### 3.2 Properties of the plugin
The following properties are currently used by the plugin:
- **Image Topic** ... The image topic to subscribe to.
- **Image Transport** ... The image transport hint of the image topic to subscribe to.
- **Resolution X** ... The image resolution in X (width) of the incoming image.
- **Resolution Y** ... The image resolution in Y (height) of the incoming image.
- **Width** ... The width of the texture, the image is projected on.
- **Height** ... The height of the texture, the image is projected on.
- **TF Frame** ... The local coordinate frame of the projected image.
- **Position** ... The position of the projected image in the local TF Frame.
- **Orientation** ... The orientation of the projected image in the local TF Frame.
- **Refresh TF Frame** ... Enables or disables refreshing the extrinsics of the projected image for each frame.
- **Border Thickness** ... The thickness of a border that is framing the projected image.
- **Border Color** ... The color of a border that is framing the projected image.
- **Alpha** ... The alpha value (transparency) of the projected image.

### 3.3 References (MLA):
- ...



## 4. rviz_plugin_display_stereoscopic - An RViz Display plugin to create a distorted stereoscopic view for VR applications.
This plugin extends RViz with a stereoscopic view. It is based on two separately rendered RViz camera views that are then distorted and concatenated.

![Image not available!](rviz_plugin_display_stereoscopic.png?raw=true "rviz_plugin_display_stereoscopic")

### 4.1 Basic use of the plugin
After adding the plugin, wait a couple of seconds to let the plugin find the proper Ogre camera.
The plugin then starts to render the stereoscopic view and publishes it on a default image topic.
You may set the image resolution and interpupillary distance (IPD) as necessary to adapt the view.
**Known issues**: Adding other relevant properties to provide better control over the intrinsics of the view is on the TODO list ^^. 

### 4.2 Properties of the plugin
The following properties are currently used by the plugin:
- **Image Topic** ... The image topic to publish the stereoscopic view.
- **Per Eye Resolution X** ... The view image resolution for one eye (e.g., a per eye resolution of 640x720 results in 1280x720 overall resolution of the full stereoscopic view image). 
- **Per Eye Resolution Y** ... The view image resolution for one eye (e.g., a per eye resolution of 640x720 results in 1280x720 overall resolution of the full stereoscopic view image).
- **Interpupillary Distance** ... The approximate distance between the two eyes, provided in meters.
- **Radial Distortion** ... The radial distortion coefficient.

### 4.3 References (MLA):
- ...



## 5. rviz_plugin_view_animated - An RViz ViewController plugin to generate animated views.
This plugin extends RViz with a view controller for animated views.

![Image not available!](rviz_plugin_view_animated.png?raw=true "rviz_plugin_view_animated")

The plugin is an extended version of the classic [**rviz_animated_view_controller**](https://github.com/ros-visualization/rviz_animated_view_controller) and [**rviz_cinematographer**](https://github.com/AIS-Bonn/rviz_cinematographer) ROS1 plugins, authored and/or maintained by Evan Flynn, Adam Leeper, Jan Razlaw, and Andy Zelenak.

### 5.1 Basic use of the plugin
After switching to the plugin, you are able to manually generate view animations ("Focus Camera") in two basic view modes (ORBIT and FPS).
To generate automated view animations publish an according ViewTrajectory message to the dedicated topic (also refer to the howto_.txt files in the package folder).
Be sure to set the proper FPS (should accord to the global RViz settings), select a valid TF Frame (Local) and set a correct Live Record Path.

### 5.2 Properties of the plugin
The following properties are currently used by the plugin:
- **TF Frame (Local)** ... The local coordinate frame of the current animated view.
- **Mode** ... The view me mode of the view controller. Currently you can select between ORBIT (the camera eye point relates to a given focus point) and FPS (the camera eye point moves freely and the focus point becomes a view direction vector).
- **Speed Profile** ... The speed profile, the view movement is based on. Available are DECLINING, RISING, WAVE, FULL, QUADRACTIC (rising), and EXPONENTIAL (declining).
- **Duration** ... The intended duration of the view movement in seconds.
- **Progress** ... The progress of the view movement in seconds.
- **FPS** ... The targeted FPS, that should accord to the global RViz settings.
- **Eye (Local)** ... The eye point as XYZ-Vector, given in the local TF Frame.
- **Focus (Local)** ... The focus point (ORBIT) or focus vector (FPS) as XYZ-Vector, given in the local TF Frame.
- **Up (Local)** ... The up vector as XYZ-Vector, given in the local TF Frame.
- **Eye (Fixed)** ... The eye point as XYZ-Vector, given in the fixed TF Frame.
- **Focus (Fixed)** ... The focus point (ORBIT) or focus vector (FPS) as XYZ-Vector, given in the fixed TF Frame.
- **Up (Fixed)** ... The up vector as XYZ-Vector, given in the fixed TF Frame.
- **Direction** ... The view direction as XYZ-Vector.
- **Orientation** ... The view orientation as Quaternion, given in the fixed TF Frame.
- **Distance** ... The view distance in ORBIT mode, measured from the eye to the focus point.
- **Frame By Frame Enabled** ... Enables or disables frame-by-frame rendering.
- **Up Vector Lock Enabled** ... Enables or disables lock of the upwards pointing vector.
- **Pose Publish Enabled** ... Enables or disables publishing the current view's pose. 
- **Mouse Control Enabled** ... Enables or disables mouse control during view animation.
- **Animation Enabled** ... Indicates an ongoing view animation, initiated by a movement trajectory.
- **Live Publish Enabled** ... Enables or disables publishing the current view as image message.
- **Live Record Enabled** ... Enables or disables recording the current view as *.AVI file to the given path (Presumes publishing of live view is enabled!).
- **Live Record Path** ... The path to save any recording of the current view as *.AVI file.
- **Live Resolution X** ... The image resolution in X (width) of the live view.
- **Live Resolution Y** ... The image resolution in Y (height) of the live view.
- **Live Background Color** ... The background color of the published or recorded live view.
- **Live Topic** ... The topic to publish the live view images.
- **Pose Topic** ... The topic to publish the current camera view pose, given in the fixed frame.
- **Completed Topic** ... The topic to publish completed messages, once the movement trajectory has finished.
- **Pause Topic** ... The topic to subscribe to pause messages.
- **Trajectory Topic** ... The topic to subscribe to view movement trajectory messages.

### 5.3 References (MLA):
- ...



### Onwards and upwards...!
