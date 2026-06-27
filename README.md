# ias_rviz_plugins
A collection of RViz plugins for the InterActiveSpace (IAS).
Please keep in mind, that this package contains open source and untested software, provided "as is".
The plugin is currently implemented for **ROS2 Jazzy Jalisco**.



## 1. rviz_plugin_display_lighting - An RViz Display plugin to enable lighting and shadows.
This plugin extends RViz with different light sources and shadow mapping.
It is an extended and refined version of the classic [**rviz_lighting**](https://github.com/mogumbo/rviz_lighting) ROS1 plugin, authored by Terence Welsh.
![Image not available!](rviz_plugin_display_lighting.png?raw=true "rviz_plugin_display_lighting")

**Known issues**:
Please note that shadow mapping is currently not supported, potentially due to an [**issue with the ogre_vendor library**](https://github.com/gazebosim/gazebo-classic/issues/2700). Please keep in mind, that this repository contains open source and untested software, provided "as is". It is work in progress as part of the InterActiveSpace (IAS).

### 1.1 Basic use of the plugin
After adding the plugin, the default light of RViz gets disabled and the plugin takes control of the "Ambient light".
Additionally, depending on what type of light you choose ("Point light", "Spot light", and "Sun light"), it affects diffuse color and specular color of the scene.
Experimenting with the available parameters should result in the desired lighting effect.
Once enabled, a marker gets published for each type of light to faciliate understanding of the lighting effects.
You should set the correct marker path, so that the markers of the different light sources get visualized correctly.
Be aware that improper mesh geometry of the environment (e.g., multiple triangles on an actually flat surface) may result in distorted lighting.

### 1.2 Properties of the plugin
The following properties are currently used by the plugin: 
- **TF Frame** ... The local coordinate frame of the lighting pose.
- **Type** ... The lighting type ("Ambient light" only, "Point light", "Spot light", or "Sun light", that is a time-dependent "Point light"). 
- **Shadows Enabled** ... Enable or disable shadow mapping (currently not supported!).
- **Marker Enabled** ... Enable or disable publishing of a visual marker for each type of lighting.
- **Ambient Color** ... The color of the ambient lighting effect for the RViz scene.
- **Diffuse Color** ... The color of the diffuse lighting effect for the RViz scene.
- **Specular Color** ... The color of the specular lighting effect for the RViz scene.
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
- **Marker Path** ... The resouce path to the marker meshes for the "Point light", "Spot light", or "Sun light".
- **Marker Topic** ... The ROS2 topic where the marker is published.
- **Color Ambient Topic** ... The ROS2 topic where incoming messages may overwrite Ambient color settings. 
- **Diffuse/Specular Topic** ... The ROS2 topic where incoming messages may overwrite Diffuse/Specular color settings.
- **Enable Topic** ... The ROS2 topic where incoming messages may enable or disable the lighting.

### 1.3 References (MLA):
- Saravanan, Gc, Govindarajan Yamuna, and S. Nandhini. "Real time implementation of RGB to HSV/HSI/HSL and its reverse color space models." 2016 International conference on communication and signal processing (ICCSP). IEEE, 2016.



## 2. rviz_plugin_display_particles - An RViz Display plugin to create particle effects.
This plugin extends RViz with different particle effects.
![Image not available!](rviz_plugin_display_particles.png?raw=true "rviz_plugin_display_particles")

### 2.1 Basic use of the plugin
After adding the plugin, you can select Presets (e.g., water leak, gas leak, smoke, fire, etc.).
Experimenting with the available parameters should result in the desired particle effect.
Be sure to select the proper TF frame for the particle emitter.

### 2.2 Properties of the plugin
The following properties are currently used by the plugin:
- **TF Frame** ... The local coordinate frame of the particle emitter.
- **Position** ... The position of the particle emitter in the local TF frame.
- **Direction** ... The direction of the particle emitter in the local TF frame.
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
- **Scope** ... The percentage of emitted particles which are randomised.
- **Keep Velocity** ... The particles either keep constant velocity or not.

### 2.3 References (MLA):
- ...

### Onwards and upwards...!
