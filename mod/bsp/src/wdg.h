#ifndef __wdg_h__
#define __wdg_h__

#include "bsp.h"

// open watchdog(enable);
int wdg_open(void);

// stop watchdog(reboot-board);
int wdg_reboot(void);

// close watchdog(disable);
int wdg_close(void);


#endif