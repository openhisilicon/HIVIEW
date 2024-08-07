
This readme introduce how to start scene_auto of each sensor:
take os04a10 as an example:

1.1 os04a10 linear mode without AIBNR:
./sample_scene param/sensor_os04a10
init success
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample
1
When We Start SceneAuto, we need to set video Pipe mode
Please input videomode index, without which we couldn't work  effectively.
0
videomode type has already been input.
(scene_load_scene_param-1817:1): load scene[1817] 
(scene_load_scene_param-1817:1): load scene[1817] 
The sceneauto is started already.
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample


1.2 os04a10 wdr mode without AIDRC:
./sample_scene param/sensor_os04a10
init success
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample
1
When We Start SceneAuto, we need to set video Pipe mode
Please input videomode index, without which we couldn't work  effectively.
1
videomode type has already been input.
(scene_load_scene_param-1817:1): load scene[1817] 
(scene_load_scene_param-1817:1): load scene[1817] 
The sceneauto is started already.
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample

1.3 os04a10 linear mode with AIBNR:
./sample_scene param/sensor_os04a10_aibnr_aidrc
init success
1.scene start
2.scene pause and reset media route
3.scene resume and set a new media type
4.exit the sample
1
When We Start SceneAuto, we need to set video Pipe mode
Please input videomode index, without which we couldn't work  effectively.
0
videomode type has already been input.
(scene_load_scene_param-1817:1): load scene[1817] 
(scene_load_scene_param-1817:1): load scene[1817] 
The sceneauto is started already.
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample


1.4 os04a10 linear mode with AIDRC:
./sample_scene param/sensor_os04a10_aibnr_aidrc
init success
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample
1
When We Start SceneAuto, we need to set video Pipe mode
Please input videomode index, without which we couldn't work  effectively.
1
videomode type has already been input.
(scene_load_scene_param-1817:1): load scene[1817] 
(scene_load_scene_param-1817:1): load scene[1817] 
The sceneauto is started already.
1.init param, set scene mode, resume
2.scene pause
3.set scene mode and resume
4.exit the sample