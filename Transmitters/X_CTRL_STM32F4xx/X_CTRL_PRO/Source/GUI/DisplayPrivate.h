#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

/*Basic*/
#include "TFT_ILI9488.h"
#include "touch.h"

#define SCREEN_CLASS TFT_ILI9488
extern SCREEN_CLASS screen;

#define TEXT_HEIGHT_1   8
#define TEXT_WIDTH_1    6
#define TextSetDefault() screen.setTextColor(screen.White,screen.Black),screen.setTextSize(1)
#define TextMid(x,textnum) (screen.width()*(x)-(TEXT_WIDTH_1*(textnum))/2.0f)
#define TextMidPrint(x,y,text) screen.setCursor(TextMid(x,strlen(text)),screen.height()*(y)-TEXT_HEIGHT_1/2.0f),screen.print(text)
#define ScreenMid_W (screen.width()/2)
#define ScreenMid_H (screen.height()/2)

/*LittleVGL*/
#include "lvgl.h"
#include "lv_conf.h"

void lv_user_init();
void lv_user_fs_init();
bool lv_obj_del_safe(lv_obj_t** obj);
int lv_label_set_text_format(lv_obj_t * label, const char *__restrict __format, ...);
lv_coord_t lv_obj_get_x_center(lv_obj_t * obj);
lv_coord_t lv_obj_get_y_center(lv_obj_t * obj);

/*Page*/
#include "PageManager.h"

typedef enum
{
    PAGE_None,
    PAGE_Home,
    PAGE_Settings,
    PAGE_SetDisplay,
    PAGE_BattInfo,
    PAGE_LuaScript,
    PAGE_FileExplorer,
    PAGE_SubAPPs,
    PAGE_MAX
} Page_Type;

extern PageManager page;

void PageRegister_Home(uint8_t pageID);
void PageRegister_BattInfo(uint8_t pageID);
void PageRegister_LuaScript(uint8_t pageID);
void PageRegister_Settings(uint8_t pageID);
void PageRegister_SetDisplay(uint8_t pageID);
void PageRegister_FileExplorer(uint8_t pageID);
void PageRegister_SubAPPs(uint8_t pageID);

/*Bar*/
void Init_Bar();
extern lv_obj_t * barStatus;
extern lv_obj_t * barNavigation;
extern lv_obj_t * appWindow;
extern lv_obj_t * btnMenu;
extern lv_obj_t * btnHome;
extern lv_obj_t * btnBack;

#define APP_WIN_HEIGHT (lv_obj_get_height(appWindow))
#define APP_WIN_WIDTH  (lv_obj_get_width(appWindow))

/*Widget*/
void Preloader_Activate(bool isact, lv_obj_t * parent);
void Keyboard_Activate(bool isact, lv_obj_t * parent, lv_obj_t * ta, lv_event_cb_t keyboard_event_cb);
lv_obj_t * Keyboard_GetObj();
void MessageBox_Activate(
    bool isact, 
    lv_obj_t * parent, 
    lv_obj_t** mbox,
    lv_coord_t w, lv_coord_t h,
    const char *text,
    const char** btns,
    lv_event_cb_t mbox_event_handler
);

#endif
