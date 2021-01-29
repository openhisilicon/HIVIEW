# linux_frame_buffer
LittlevGL configured to work with /dev/fb0 on Linux

make CC=arm-hisiv510-linux-gcc



I add ¡°LV_COLOR_FMT_RGB1555¡± to support it, Your lib may add them too.

¡ª lv_color.h
typedef union
{
struct
{
#if LV_COLOR_16_SWAP == 0
#if LV_COLOR_FMT_RGB1555
uint16_t blue : 5;
uint16_t green : 5;
uint16_t red : 5;
uint16_t alpha : 1;
#else
uint16_t blue : 5;
uint16_t green : 6;
uint16_t red : 5;
#endif
#else
uint16_t green_h : 3;
uint16_t red : 5;
uint16_t blue : 5;
uint16_t green_l : 3;
#endif
} ch;
uint16_t full;
} lv_color16_t;

#elif LV_COLOR_DEPTH == 16
#if LV_COLOR_16_SWAP == 0

#if LV_COLOR_FMT_RGB1555
//RGB1555
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{b8 >> 3, g8 >> 3, r8 >> 3, 1}})
static inline lv_color_t lv_color_make(uint8_t r8, uint8_t g8, uint8_t b8)
{
lv_color_t color;
color.ch.blue = (uint16_t)(b8 >> 3);
color.ch.green = (uint16_t)(g8 >> 3);
color.ch.red = (uint16_t)(r8 >> 3);
color.ch.alpha = 1;
return color;
}