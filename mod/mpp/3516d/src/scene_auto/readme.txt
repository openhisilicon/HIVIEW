1. This scene sample can only be run when vi-isp is running;
2. Diffierent sensor have diffierent config and diffierent mainapp.
3. use ini2source can generate part of source of scene from inicfg file
   (Attention: We have already generate file. So you don't need to do it again!!!!)
4. go to out/sensor_bin/sensor_XXX/, then execute the mainapp


Currently:
Sensor imx290, we have create linear and wdr mode sample.
When you start a linear mode in Vi Pipe 0, you execute the mainapp, and in the app then input 0;
When you start a wdr mode in Vi pipe 0 and 1, you can execute the mainapp, and in the app then input 1;

Sensor imx334 and 334avs_4ch, only have linear mode sample.
Just start the corresponding media route, and execute the mainapp, and in the app then input 0.