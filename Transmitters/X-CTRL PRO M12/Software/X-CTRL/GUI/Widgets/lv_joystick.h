#ifndef LV_JOYSTICK_H
#define LV_JOYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
    
#define LV_JOYSTICK_MAX_VALUE_DEFAULT 1000
    
enum{
    LV_JOYSTICK_STYLE_MAIN,
    LV_JOYSTICK_STYLE_BASEPLATE,
    LV_JOYSTICK_STYLE_LINE,
};
typedef uint8_t lv_joystick_style_t;

enum{
    LV_JOYSTICK_LINE_HOR,
    LV_JOYSTICK_LINE_VER,
};
typedef uint8_t lv_joystick_line_t;

typedef struct{
    lv_cont_ext_t cont;
    lv_coord_t size;
    lv_coord_t max_value;
    lv_point_t line_hor_points[2];
    lv_point_t line_ver_points[2];
}lv_joystick_ext_t;

lv_obj_t * lv_joystick_creat(lv_obj_t * par, const lv_obj_t * copy);
void lv_joystick_set_size(lv_obj_t * joystick, lv_coord_t size);
void lv_joystick_set_style(lv_obj_t * joystick, lv_cont_style_t type, lv_style_t * style);
void lv_joystick_set_max_value(lv_obj_t * joystick, lv_coord_t max_value);
void lv_joystick_set_value(lv_obj_t * joystick, lv_coord_t x, lv_coord_t y);

lv_obj_t * lv_joystick_get_base_plate(lv_obj_t * joystick);
lv_obj_t * lv_joystick_get_line(lv_obj_t * joystick, lv_joystick_line_t type);

#ifdef __cplusplus
}
#endif

#endif
