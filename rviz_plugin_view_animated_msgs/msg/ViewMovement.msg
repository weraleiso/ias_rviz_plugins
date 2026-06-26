string view_tf_frame

uint8 view_mode
uint8 ORBIT = 0
uint8 FPS   = 1

uint8 view_speed_profile
uint8 DECLINING   = 0
uint8 RISING      = 1
uint8 WAVE        = 2
uint8 FULL        = 3
uint8 QUADRATIC   = 4
uint8 EXPONENTIAL = 5

std_msgs/Float32 view_duration
std_msgs/Float32 view_target_fps
bool view_frame_by_frame_enabled
bool view_vector_up_lock_enabled
bool view_pose_enabled
bool view_live_enabled
bool view_mouse_enabled

geometry_msgs/PointStamped view_eye
geometry_msgs/PointStamped view_focus
geometry_msgs/Vector3Stamped view_up
