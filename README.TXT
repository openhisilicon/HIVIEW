// directory tree:  
.  
├── bin         (output bin)  
├── build       (build config)  
├── fw          (framework)  
│   ├── cfifo   (cycle buffer)  
│   ├── nm      (netmsg)  
│   ├── cjson   (cjson)  
│   └── comm    (comm)  
├── inc         (top include)  
├── lib         (output library)  
└── mod         (module)  
    ├── bsp     (sys init && netinf && syscfg && log)  
    ├── cam     (mipi&isp&af)  
    ├── codec   (vi&vpss&venc&vdec&vo)  
    ├── svp     (smart vison platform)  
    ├── rec     (video file to storage)  
    ├── netbox  (for ftp ntp ddns ...)  
    ├── mpp     (hisi mpp)  
    ├── joint   (event joint)  
    ├── gui     (gui base hifb)  
    ├── onvif   (onvif nvt)  
    ├── rtsps   (rtsp server)  
    └── webs    (web server)  
  
  
// system structure:  
  
+------------+       +------------+      +-----------+  
|    bsp     |       |    cam     |      |    svp    |  
+------------+       +------------+      +-----------+              req +------------+ sub  
    rep                   rep             pub  rep                <-----+    gui     +------>  
     ^                     ^               ^    ^                 |     +------------+      |  
     |                     |               |    |                 |                         |  
     |                     |   +-----------+    |                 |                         |  
     |                     |   |                |                 | req +------------+ sub  |  
     +---------------------+--------------------+-----------------<-----+    webs    +------>  
     |                     |   |                |                 |     +------------+      |  
     |                     |   |                |                 |                         |  
     |                     |   |                |                 |                         |  
     |                     |   |                |                 | req +------------+ sub  |  
     v                     v   +                v                 <-----+    rtsps   +------>  
     rep                  rep sub              rep                      +-------+--+-+      |  
+------------+      +------------+       +------------+                         |  |        |  
|    rec     |      |   joint    |       |   codec    |cfifo<-------------------+  |        |  
+--+--+------+      +------------+       +------------+    ^                       |        |  
   |  |                   pub                              |                       |        |  
   |  |                    ^                               |                       |        |  
   |  |                    |                               |                       |        |  
   |  |                    +----------------------------------------------------------------+  
   |  |                                                    |                       |  
   |  +----------------------------------------------------+                       |  
   |                                                                               |  
   |                                  +-------------+                              |  
   +--------------------------------> | shmMsg/file | <----------------------------+  
                                      +-------------+  
  
  
  
// startup sequence:  
  
  BSP   CAM  CODEC  SVP   REC  NETBOX  JOINT RTSPS  GUI    WEBS  
   +     +     +     +     +     +      +      +     +      +  
   |     |     |     |     |     |      |      |     |      |  
   |     |     |     |     |     |      |      |     |      |  
+--v-----v-----v-----v-----v-----v------v------v-----v------v-----> time;  
  
  
// compile:  
  
source build/x86   
make  
  
  
// run:  
  
export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/bsp.exe bsp_def.json bsp_parm.json  
  
export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/cam.exe  cam_parm.json  

export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/codec.exe codec_parm.json

export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/webs.exe

<end.>  