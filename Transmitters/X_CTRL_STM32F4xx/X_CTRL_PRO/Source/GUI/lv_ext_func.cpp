#include "DisplayPrivate.h"

bool lv_obj_del_safe(lv_obj_t** obj)
{
    bool ret = false;
    if(*obj != NULL)
    {
        lv_obj_del(*obj);
        *obj = NULL;
        ret = true;
    }
    return ret;
}

extern "C" {
#include <stdio.h>
#include <stdarg.h>
}

int lv_label_set_text_format(lv_obj_t * label, const char *__restrict __format, ...)
{
    char buff[50];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(buff, sizeof(buff), __format, args);
    va_end(args);
    
    lv_label_set_text(label, buff);

    return ret_status;
}

lv_coord_t lv_obj_get_x_center(lv_obj_t * obj)
{
    return (lv_obj_get_x(obj) + lv_obj_get_width(obj) / 2);
}

lv_coord_t lv_obj_get_y_center(lv_obj_t * obj)
{
    return (lv_obj_get_y(obj) + lv_obj_get_height(obj) / 2);
}
