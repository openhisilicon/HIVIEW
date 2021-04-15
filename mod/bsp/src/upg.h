#ifndef __upg_h__
#define __upg_h__

#include "bsp.h"

// ---------------------------//
// 384K(uboot),128K(uargs),512K(factory),5M(kernel),-(app)
// ubootbin     none      384k; [uboot bin]
// bootargs     none      128k; [uboot args]
// factory      jffs2     512k; [startup, factoryfile, checkapp-rescue]
// kernel-cpio  none      5M;   [kernel, rootfs]
// app          jffs2     -M;   [application, cfgfile]
// ---------------------------//

// ---------------------------//
// install.sh  1, install shell;
// kernel;     1, change bootcmd-rescue; 2, write kernel; 3, restore bootcmd;
// file.tgz;   1, check app free space; 2, write file to app;
// ---------------------------//

// start upgrade;
int upg_start(char *path, int(*cb)(int progress, void *u), void *u);

// isruning;
int upg_runing(void);

#endif