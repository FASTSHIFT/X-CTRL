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
#include "lv_ex_conf.h"

void lv_user_init();
void lv_user_fs_init();
bool lv_obj_del_safe(lv_obj_t** obj);
int lv_label_set_text_format(lv_obj_t * label, const char *__restrict __format, ...);

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
    PAGE_MAX
} Page_Type;

extern PageManager page;

void PageRegister_Home(uint8_t pageID);
void PageRegister_BattInfo(uint8_t pageID);
void PageRegister_LuaScript(uint8_t pageID);
void PageRegister_Settings(uint8_t pageID);
void PageRegister_SetDisplay(uint8_t pageID);

/*Bar*/
void Init_Bar();
extern lv_obj_t *barStatus;
extern lv_obj_t *barNavigation;
extern lv_obj_t *btnMenu;
extern lv_obj_t *btnHome;
extern lv_obj_t *btnBack;

#define page_height (lv_disp_get_ver_res(NULL) - lv_obj_get_height(barStatus) - lv_obj_get_height(barNavigation))
#define page_width  (lv_disp_get_hor_res(NULL))

/*Types*/
typedef enum{
    TYPE_None,
    TYPE_PageJump,
    TYPE_FuncCall,
}Func_Type;

#endif
