#ifndef __APP_TYPE_H
#define __APP_TYPE_H

#include "lvgl.h"
#include "lv_conf.h"

/*Types*/
typedef enum{
    TYPE_None,
    TYPE_PageJump,
    TYPE_FuncCall,
}Func_Type;

/*存放APP图标和标签的容器*/
typedef struct
{
    /*手动*/
    const void* img_dsc;//图片
    const char *lable_text;     //标签文本
    Func_Type type;             //跳转类型
    int param;                  //附带参数
    
    /*自动*/
    lv_obj_t* cont;             //标签容器
    lv_obj_t* imgbtn;           //图标按钮控件
    lv_obj_t* label;            //标签控件
} APP_TypeDef;

void Creat_APP(APP_TypeDef &app, lv_obj_t * parent, uint8_t index, lv_event_cb_t imgbtn_event_handler);

#endif
