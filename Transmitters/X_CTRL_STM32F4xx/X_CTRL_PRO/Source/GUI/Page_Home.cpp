#include "FileGroup.h"
#include "DisplayPrivate.h"

#include "IMG/ImgSettings.c"
#include "IMG/ImgCode.c"
#include "IMG/ImgGame.c"

LV_IMG_DECLARE(ImgSettings);
LV_IMG_DECLARE(ImgCode);
LV_IMG_DECLARE(ImgGame);

static lv_obj_t * tabviewHome;
static lv_obj_t * tabGrp[2];
static lv_obj_t * preloadAPP;

enum{
    APP_Settings,
    APP_Editor,
    APP_Game,
    APP_MAX
};

typedef struct{
    lv_obj_t* imgbtn;
    lv_obj_t* label;
    uint8_t page_id;
    const lv_img_dsc_t* img_dsc;
    const char *lable_text;
    lv_align_t align;
    lv_coord_t x_mod;
    lv_coord_t y_mod;
}APP_TypeDef;

static APP_TypeDef APP_Grp[APP_MAX] = {
    {NULL, NULL, PAGE_Settings,  &ImgSettings, "Settings", LV_ALIGN_OUT_BOTTOM_LEFT,   20, 20},
    {NULL, NULL, PAGE_LuaScript, &ImgCode,     "Editor",   LV_ALIGN_OUT_BOTTOM_MID,     0, 20},
    {NULL, NULL, 0,              &ImgGame,     "Game",     LV_ALIGN_OUT_BOTTOM_RIGHT, -20, 20}
};

static void imgbtn1_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        for(int i = 0; i < APP_MAX; i++)
        {
            if(obj == APP_Grp[i].imgbtn)
            {
                page.PageChangeTo(APP_Grp[i].page_id);
            }
        }
    }
}

static void APP_Creat(APP_TypeDef &app, lv_obj_t * parent)
{
    static lv_style_t style_pr;
    lv_style_copy(&style_pr, &lv_style_plain_color);
    style_pr.body.main_color = LV_COLOR_WHITE;
    style_pr.body.grad_color = LV_COLOR_WHITE;
    style_pr.image.color = LV_COLOR_WHITE;
    style_pr.image.intense = LV_OPA_50;
    style_pr.text.color = LV_COLOR_BLACK;

    /*Create an Image button*/
    app.imgbtn = lv_imgbtn_create(parent, NULL);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_REL, app.img_dsc);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_PR, app.img_dsc);
    lv_imgbtn_set_style(app.imgbtn, LV_BTN_STATE_PR, &style_pr);
    lv_imgbtn_set_style(app.imgbtn, LV_BTN_STATE_TGL_PR, &style_pr);
    lv_imgbtn_set_toggle(app.imgbtn, false);
    lv_obj_align(app.imgbtn, barStatus, app.align, app.x_mod, app.y_mod);
    
    lv_obj_set_event_cb(app.imgbtn, imgbtn1_event);

    /*Create a label on the Image button*/
    static lv_style_t style_label;
    lv_style_copy(&style_label, &lv_style_plain);
    style_label.text.color = LV_COLOR_BLACK;
    
    app.label = lv_label_create(parent, NULL);
    lv_label_set_text(app.label, app.lable_text);
    lv_label_set_style(app.label, LV_LABEL_STYLE_MAIN, &style_label);
    lv_obj_align(app.label, app.imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}

static void Preload_Creat(lv_obj_t** preload, lv_obj_t * parent)
{
    /*Create a style for the Preloader*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.width = 10;                         /*10 px thick arc*/
    style.line.color = lv_color_hex3(0x258);       /*Blueish arc color*/

    style.body.border.color = lv_color_hex3(0xBBB); /*Gray background color*/
    style.body.border.width = 10;
    style.body.padding.left = 0;

    /*Create a Preloader object*/
    *preload = lv_preload_create(parent, NULL);
    lv_obj_set_size(*preload, 50, 50);
    lv_obj_align(*preload, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_preload_set_style(*preload, LV_PRELOAD_STYLE_MAIN, &style);
    lv_preload_set_type(*preload, LV_PRELOAD_TYPE_FILLSPIN_ARC);
}

static void Page_Creat(lv_obj_t** tabview)
{
    static lv_style_t style_bg;
    lv_style_copy(&style_bg, &lv_style_plain_color);
    style_bg.body.main_color = LV_COLOR_WHITE;
    style_bg.body.grad_color = LV_COLOR_GRAY;
    
     /*Create a Tab view object*/
    *tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_set_size(*tabview, lv_obj_get_width(barStatus), page_height);
    lv_obj_align(*tabview, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_tabview_set_style(*tabview, LV_TABVIEW_STYLE_BG, &style_bg);
    lv_tabview_set_btns_pos(*tabview, LV_TABVIEW_BTNS_POS_BOTTOM);
    lv_tabview_set_btns_hidden(*tabview, true);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    tabGrp[0] = lv_tabview_add_tab(*tabview, "APP");
    tabGrp[1] = lv_tabview_add_tab(*tabview, "x");
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    static bool isInit = false;
    if(!isInit)
    {
        Page_Creat(&tabviewHome);
        isInit = true;
    }
    
    for(int i = 0; i < APP_MAX; i++)
    {
        APP_Creat(APP_Grp[i], tabGrp[0]);
    }
    
    //Preload_Creat(&preloadAPP, lv_scr_act());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    for(int i = 0; i < APP_MAX; i++)
    {
        lv_obj_del(APP_Grp[i].imgbtn);
        lv_obj_del(APP_Grp[i].label);
        APP_Grp[i].imgbtn = APP_Grp[i].label = NULL;
    }
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Home(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
