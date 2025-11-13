#ifndef OLED_H
#define OLED_H
#include "lvgl.h"

extern lv_disp_t *disp;
extern lv_obj_t *label;
void oled_init(void);
void lvgl_init();
void lvgl_write(lv_disp_t *disp);
#endif