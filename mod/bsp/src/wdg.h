#ifndef __wdg_h__
#define __wdg_h__

#include "bsp.h"

// open watchdog(enable);
int wdg_open(int to);

// stop watchdog(reset-board);
int wdg_stop(void);

// close watchdog(disable);
int wdg_close(void);


#endif