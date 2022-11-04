/**
 * @file mouse_hid.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "mouse_hid.h"
#if USE_MOUSE_HID != 0

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
int mouse_hid_fd;
int mouse_hid_root_x;
int mouse_hid_root_y;
int mouse_hid_button;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the mouse
 */
void mouse_hid_init(void)
{
    mouse_hid_fd = open("/dev/input/event0", O_RDWR|O_NOCTTY|O_NDELAY);
    if (mouse_hid_fd == -1) {
        perror("unable open mouse event:");
        return;
    }

    fcntl(mouse_hid_fd, F_SETFL, O_ASYNC|O_NONBLOCK);

    mouse_hid_root_x = 0;
    mouse_hid_root_y = 0;
    mouse_hid_button = LV_INDEV_STATE_REL;
}

static void(*_hid_cb)(struct input_event* in);
int mouse_hid_cb_set(void(*cb)(struct input_event* in))
{
  _hid_cb = cb;
}

/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool mouse_hid_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    struct input_event in;

    while(read(mouse_hid_fd, &in, sizeof(struct input_event)) > 0) {
        if (in.type == 2) {
            if (in.code == 0)
                mouse_hid_root_x += in.value;
            if (in.code == 1)
                mouse_hid_root_y += in.value;
        } else if (in.type == 1) {
            if (in.code == 272) {
                if (in.value == 0)
                    mouse_hid_button = LV_INDEV_STATE_REL;
                if (in.value == 1)
                    mouse_hid_button = LV_INDEV_STATE_PR;
            }
            
            if(_hid_cb)
            {
              _hid_cb(&in);
            }
        }
    }

    if (mouse_hid_root_x < 0)
        mouse_hid_root_x = 0;
    if (mouse_hid_root_y < 0)
        mouse_hid_root_y = 0;
    if (mouse_hid_root_x >= LV_HOR_RES)
        mouse_hid_root_x = LV_HOR_RES - 1;
    if (mouse_hid_root_y >= LV_VER_RES)
        mouse_hid_root_y = LV_VER_RES - 1;

    /*Store the collected data*/
    data->point.x = mouse_hid_root_x;
    data->point.y = mouse_hid_root_y;
    data->state = mouse_hid_button;

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
