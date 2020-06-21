#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

/*Basic*/
#include "Adafruit_ST7789/Adafruit_ST7789.h"

typedef Adafruit_ST7789 SCREEN_CLASS;

extern SCREEN_CLASS screen;

#define TEXT_HEIGHT_1   8
#define TEXT_WIDTH_1    6
#define TextSetDefault() screen.setTextColor(screen.White,screen.Black),screen.setTextSize(1)
#define TextMid(x,textnum) (screen.width()*(x)-(TEXT_WIDTH_1*(textnum))/2.0f)
#define TextMidPrint(x,y,text) screen.setCursor(TextMid(x,strlen(text)),screen.height()*(y)-TEXT_HEIGHT_1/2.0f),screen.print(text)
#define ScreenMid_W (screen.width()/2)
#define ScreenMid_H (screen.height()/2)

void DisplayError_Init();
void Display_Init();
void Display_Update();

/*Page*/
#include "PageManager/PageManager.h"
typedef enum
{
    /*保留*/
    PAGE_NONE,
    /*用户页面*/
    PAGE_Home,
    PAGE_CtrlPage,
    PAGE_Handshake,
    PAGE_HandshakeAuto,
    PAGE_ChannelCfg,
    PAGE_ChannelRevCfg,
    PAGE_JoystickCfg,
    PAGE_JoystickCurveCfg,
    PAGE_ModelCfg,
    PAGE_RadioCfg,
    PAGE_Scanner,
    PAGE_GyroscopeCfg,
    PAGE_BluetoothCfg,
    PAGE_MiscCfg,
    /*保留*/
    PAGE_MAX
} Page_Type;

extern PageManager page;
void Page_Init();
void Page_Delay(uint32_t ms);
void Page_ReturnHome();
#define PageDelay(ms) Page_Delay(ms)

/*LVGL*/
#include "lvgl/lvgl.h"

/*Custom Widgets*/
#include "GUI/Widgets/lv_joystick.h"

#define LV_ANIM_TIME_DEFAULT    200
#define LV_SYMBOL_DEGREE_SIGN   "\xC2\xB0"
#define LV_COLOR_ARMY_GREEN     LV_COLOR_MAKE(2,94,33)

void lv_port_disp_init();
bool lv_obj_del_safe(lv_obj_t** obj);
void lv_label_set_text_add(lv_obj_t * label, const char * text);
lv_coord_t lv_obj_get_x_center(lv_obj_t * obj);
lv_coord_t lv_obj_get_y_center(lv_obj_t * obj);
void lv_obj_set_color(lv_obj_t * obj, lv_color_t color);
void lv_table_set_align(lv_obj_t * table, lv_label_align_t align);
lv_obj_t * lv_win_get_label(lv_obj_t * win);
void lv_sw_set_state(lv_obj_t * sw, bool en);
void lv_obj_add_anim(
    lv_obj_t * obj, lv_anim_t * a,
    lv_anim_exec_xcb_t exec_cb, 
    int32_t start, int32_t end,
    uint16_t time = LV_ANIM_TIME_DEFAULT,
    lv_anim_ready_cb_t ready_cb = NULL,
    lv_anim_path_cb_t path_cb = lv_anim_path_ease_in_out
);
#define LV_OBJ_ADD_ANIM(obj,attr,target,time)\
do{\
    static lv_anim_t a;\
    lv_obj_add_anim(\
        (obj), &a,\
        (lv_anim_exec_xcb_t)lv_obj_set_##attr,\
        lv_obj_get_##attr(obj),\
        (target),\
        (time)\
    );\
}while(0)

#define LV_LABEL_SET_FONT(label,label_font,label_color)\
    static lv_style_t style_##label;\
    style_##label = *lv_label_get_style(label, LV_LABEL_STYLE_MAIN);\
    LV_FONT_DECLARE(label_font);\
    style_##label.text.font = &label_font;\
    style_##label.text.color = (label_color);\
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &(style_##label))


/*StatusBar*/
void StatusBar_Init();
lv_coord_t StatusBar_GetHeight();
lv_obj_t * StatusBar_GetObj();

/*AppWindow*/
void AppWindow_Init(lv_obj_t * par = lv_scr_act());
lv_obj_t * AppWindow_GetCont(uint8_t pageID);
lv_coord_t AppWindow_GetHeight();
lv_coord_t AppWindow_GetWidth();
#define APP_WIN_HEIGHT AppWindow_GetHeight()
#define APP_WIN_WIDTH  AppWindow_GetWidth()

#endif
