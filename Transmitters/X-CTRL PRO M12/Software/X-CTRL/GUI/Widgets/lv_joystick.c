#include "lv_joystick.h"

lv_obj_t * lv_joystick_creat(lv_obj_t * par, const lv_obj_t * copy)
{
    /*Ò¡¸ËÈÝÆ÷*/
    lv_obj_t * cont_main = lv_cont_create(par, NULL);
    
    lv_obj_allocate_ext_attr(cont_main, sizeof(lv_joystick_ext_t));
    lv_joystick_ext_t * ext = lv_obj_get_ext_attr(cont_main);
    
    ext->max_value = LV_JOYSTICK_MAX_VALUE_DEFAULT;
    ext->size = lv_obj_get_width(cont_main);
    
    /*Ò¡¸ËÒÆ¶¯µ×°å*/
    lv_obj_t * cont_base_plate = lv_cont_create(cont_main, NULL);
    
    /*µ×°åÊ®×Ö¼Ü*/
    /*hor ºáÏß*/
    lv_line_create(cont_base_plate, NULL);

    /*ver ÊúÏß*/
    lv_line_create(cont_base_plate, NULL);
    
    lv_joystick_set_size(cont_main, ext->size);
    
    return cont_main;
}

void lv_joystick_set_size(lv_obj_t * joystick, lv_coord_t size)
{
    lv_joystick_ext_t * ext = lv_obj_get_ext_attr(joystick);
    ext->size = size;
    
    /*Ò¡¸ËÈÝÆ÷*/
    lv_obj_set_size(joystick, ext->size, ext->size);
    
    /*Ò¡¸ËÒÆ¶¯µ×°å*/
    lv_obj_t * cont_baseplate = lv_joystick_get_base_plate(joystick);
    lv_obj_set_size(cont_baseplate, 2 * ext->size, 2 * ext->size);
    lv_obj_align(cont_baseplate, NULL, LV_ALIGN_CENTER, 0, 0);
    
    /*hor ºáÏß*/
    ext->line_hor_points[0].x = 0;
    ext->line_hor_points[0].y = 0;
    ext->line_hor_points[1].x = 2 * ext->size;
    ext->line_hor_points[1].y = 0;
    lv_obj_t * line_hor = lv_joystick_get_line(joystick, LV_JOYSTICK_LINE_HOR);
    lv_line_set_points(line_hor, ext->line_hor_points, 2);
    lv_obj_align(line_hor, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
    
    /*ver ÊúÏß*/
    ext->line_ver_points[0].x = 0;
    ext->line_ver_points[0].y = 0;
    ext->line_ver_points[1].x = 0;
    ext->line_ver_points[1].y = 2 * ext->size;
    lv_obj_t * line_ver = lv_joystick_get_line(joystick, LV_JOYSTICK_LINE_VER);
    lv_line_set_points(line_ver, ext->line_ver_points, 2);
    lv_obj_align(line_ver, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

void lv_joystick_set_max_value(lv_obj_t * joystick, lv_coord_t max_value)
{
    lv_joystick_ext_t * ext = lv_obj_get_ext_attr(joystick);
    ext->max_value = LV_MATH_ABS(max_value);
}

static long lv_joystick_map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void lv_joystick_set_value(lv_obj_t * joystick, lv_coord_t x, lv_coord_t y)
{
    lv_joystick_ext_t * ext = lv_obj_get_ext_attr(joystick);

    x = (x > ext->max_value) ? ext->max_value : x;
    x = (x < -ext->max_value) ? -ext->max_value : x;
    
    y = (y > ext->max_value) ? ext->max_value : y;
    y = (y < -ext->max_value) ? -ext->max_value : y;

    int16_t new_x =  lv_joystick_map(x, -ext->max_value, ext->max_value, -ext->size, 0);
    int16_t new_y = -lv_joystick_map(y, -ext->max_value, ext->max_value, 0, ext->size);
    
    lv_obj_set_pos(lv_joystick_get_base_plate(joystick), new_x, new_y);
}

void lv_joystick_set_style(lv_obj_t * joystick, lv_cont_style_t type, lv_style_t * style)
{
    switch(type)
    {
        case LV_JOYSTICK_STYLE_MAIN:
            lv_cont_set_style(joystick, LV_CONT_STYLE_MAIN, style);
            break;
        case LV_JOYSTICK_STYLE_BASEPLATE:
            lv_cont_set_style(lv_joystick_get_base_plate(joystick), LV_CONT_STYLE_MAIN, style);
            break;
        case LV_JOYSTICK_STYLE_LINE:
            lv_line_set_style(lv_joystick_get_line(joystick, LV_JOYSTICK_LINE_HOR), LV_LINE_STYLE_MAIN, style);
            lv_line_set_style(lv_joystick_get_line(joystick, LV_JOYSTICK_LINE_VER), LV_LINE_STYLE_MAIN, style);
            break;
        default:
            break;
    }
}

lv_obj_t * lv_joystick_get_base_plate(lv_obj_t * joystick)
{
    lv_obj_t * base_plate = lv_obj_get_child(joystick, NULL);
    return base_plate;
}

lv_obj_t * lv_joystick_get_line(lv_obj_t * joystick, lv_joystick_line_t type)
{
    lv_obj_t * line = NULL;
    if(type == LV_JOYSTICK_LINE_HOR)
    {
        line = lv_obj_get_child(lv_joystick_get_base_plate(joystick), NULL);
    }
    else if(type == LV_JOYSTICK_LINE_VER)
    {
        line = lv_obj_get_child(lv_joystick_get_base_plate(joystick), NULL);
        line = lv_obj_get_child(lv_joystick_get_base_plate(joystick), line);
    }
    return line;
}

