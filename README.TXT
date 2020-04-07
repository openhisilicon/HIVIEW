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
    ├── codec   (isp&&venc&vdec&vo)  
    ├── svp     (smart vison platform)  
    ├── rec     (video file to storage)  
    ├── user1   (user custom module)
    ├── mpp     (hisi mpp)  
    ├── app     (event joint && application)  
    ├── gui     (gui base hifb)  
    ├── onvif   (onvif nvt)  
    ├── rtsps   (rtsp server)  
    └── webs    (web server)  
  
  
// system structure:  
  
+------------+    +------------+         +-----------+  
|    app     |    |   user1    |         |    svp    |             req +------------+  
+------------+    +------------+         +-----------+           <-----+    gui     |  
   pub  rep          pub   rep            pub    rep             |     +------------+  
    ^    ^            ^     ^              ^      ^              |  
    |    |            |     |              |      |              |  
    |    |            |     |              |      |              | req +------------+  
    +------------------------------------------------------------------+    webs    |  
    |    |            |     |              |      |              |     +------------+  
    |    |            |     |              |      |              |  
    |    |            |     |              |      |              | req +------------+  
    v    v            v     v              v      v              <-----+    rtsps   |  
   pub  rep          pub   rep            pub    rep                   +-------+--+-+  
+------------+    +------------+        +------------+                         |  |  
|    bsp     |    |    codec   |cfifo <-|     rec    |file <-------------------+  |  
+------------+    +------------+  ^     +------------+                            |  
                                  |                                               |  
                                  +-----------------------------------------------+  
  
+-----------------------------------------------------------------------------------+  
|                                linux && HisiSDK                                   |  
+-----------------------------------------------------------------------------------+  
  
  
  
  
// startup sequence:  
  
  BSP  CODEC  SVP   REC  USER1  APP   RTSPS  GUI    WEBS  
   +     +     +     +     +      +      +     +      +  
   |     |     |     |     |      |      |     |      |  
   |     |     |     |     |      |      |     |      |  
+--v-----v-----v-----v-----v------v------v-----v------v-----> time;  
  
  
// compile:  
  
source build/x86   
make  
./ins.sh  
  
// run:  
  
export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/bsp.exe bsp_def.json bsp_parm.json  
  
export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/codec.exe codec_parm.json  
  
export LD_LIBRARY_PATH=./lib/x86  
./bin/x86/webs.exe  
  
<end.>  