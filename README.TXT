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
    ├── app     (application && gui)
    ├── onvif   (onvif nvt && nvc)  
    ├── rtsps   (rtsp server && client)
    └── webs    (web server)  
  
  
// system structure:  
  
+------------+    +------------+         +-----------+  
|    app     |    |   user1    |         |    svp    |             req +------------+
+------------+    +------------+         +-----------+           <-----+   onvif    |
   pub  rep          pub   rep            pub    rep             |     +------------+
    ^    ^            ^     ^              ^      ^              |  
    |    |            |     |              |      |              |  
    |    |            |     |              |      |              | req +------------+  
    +----+------------+-----+--------------+------+--------------+-----+    webs    |  
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
  
  BSP  CODEC  SVP   REC   USER1  RTSPS  WEBS ONVIF  APP
   +     +     +     +     +       +     +     +     +  
   |     |     |     |     |       |     |     |     |  
   |     |     |     |     |       |     |     |     |  
+--v-----v-----v-----v-----v-------v-----v-----v-----v----> time;
  
  
// compile:  
  
source build/3516d  
 
make 

<end.>
