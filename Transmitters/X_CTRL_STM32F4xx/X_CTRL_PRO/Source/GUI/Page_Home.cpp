#include "FileGroup.h"
#include "DisplayPrivate.h"

#include "IMG/ImgSettings.c"
#include "IMG/ImgCode.c"
#include "IMG/ImgGame.c"
#include "IMG/ImgExplorer.c"
#include "IMG/ImgFiles.c"
#include "IMG/ImgInternet.c"
#include "IMG/ImgMessage.c"
#include "IMG/ImgPlay.c"
#include "IMG/ImgUSB.c"

static lv_obj_t * tabviewHome;

typedef struct
{
    const char* text;
    lv_obj_t * tab;
} AppCont_TypeDef;

static uint8_t tabHomeIndex = 1;
static AppCont_TypeDef AppCont_Grp[] =
{
    {""},
    {""},
    {""}
};

typedef struct
{
    const lv_img_dsc_t* img_dsc;
    const char *lable_text;
    Func_Type type;
    int param;
    lv_obj_t* cont;
    lv_obj_t* imgbtn;
    lv_obj_t* label;
} APP_TypeDef;

static APP_TypeDef APP_Grp[] =
{
    {&ImgSettings, "Settings", TYPE_PageJump, PAGE_Settings},
    {&ImgCode,     "Editor",   TYPE_PageJump, PAGE_LuaScript},
    {&ImgGame,     "Game"},
    {&ImgExplorer, "Explorer"},
    {&ImgFiles,    "Files"},
    {&ImgInternet, "Internet"},
    {&ImgMessage,  "Message"},
    {&ImgPlay,     "Player"},
    {&ImgUSB,      "USB"},
};

static void imgbtn1_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        for(int i = 0; i < __Sizeof(APP_Grp); i++)
        {
            if(obj == APP_Grp[i].imgbtn && APP_Grp[i].param != PAGE_None)
            {
                page.PageChangeTo(APP_Grp[i].param);
            }
        }
    }
}

static void Creat_APP(APP_TypeDef &app, lv_obj_t * parent, uint8_t index)
{
    if(index > 8)
        return;
    
    /*creat cont*/
    static lv_style_t style_cont;
    lv_style_copy(&style_cont, &lv_style_plain);
    style_cont.body.opa = LV_OPA_TRANSP;
    app.cont = lv_cont_create(parent, NULL);
    lv_cont_set_style(app.cont, LV_CONT_STYLE_MAIN, &style_cont);

    /*imgbtn style*/
    static lv_style_t style_pr;
    lv_style_copy(&style_pr, &lv_style_plain_color);
    style_pr.body.main_color = LV_COLOR_WHITE;
    style_pr.body.grad_color = LV_COLOR_WHITE;
    style_pr.image.color = LV_COLOR_WHITE;
    style_pr.image.intense = LV_OPA_50;
    style_pr.text.color = LV_COLOR_BLACK;

    /*Create an Image button*/
    app.imgbtn = lv_imgbtn_create(app.cont, NULL);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_REL, app.img_dsc);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_PR, app.img_dsc);
    lv_imgbtn_set_style(app.imgbtn, LV_BTN_STATE_PR, &style_pr);
    lv_imgbtn_set_style(app.imgbtn, LV_BTN_STATE_TGL_PR, &style_pr);
    lv_imgbtn_set_toggle(app.imgbtn, false);

    lv_obj_set_event_cb(app.imgbtn, imgbtn1_event);

    /*Create a label on the Image button*/
    static lv_style_t style_label;
    lv_style_copy(&style_label, &lv_style_plain);
    style_label.text.color = LV_COLOR_BLACK;

    app.label = lv_label_create(app.cont, NULL);
    lv_label_set_text(app.label, app.lable_text);
    lv_label_set_style(app.label, LV_LABEL_STYLE_MAIN, &style_label);
    lv_obj_align(app.label, app.imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    //lv_obj_set_auto_realign(app.cont, true);
    lv_cont_set_fit(app.cont, LV_FIT_TIGHT);
    lv_cont_set_layout(app.cont, LV_LAYOUT_COL_M);

    #define MOD_OFFSET 20
    lv_coord_t mod[3] = {MOD_OFFSET, 0, -MOD_OFFSET};
    lv_align_t align[] = {
        LV_ALIGN_IN_TOP_LEFT,
        LV_ALIGN_IN_TOP_MID,
        LV_ALIGN_IN_TOP_RIGHT,
        LV_ALIGN_IN_LEFT_MID,
        LV_ALIGN_CENTER,
        LV_ALIGN_IN_RIGHT_MID,
        LV_ALIGN_IN_BOTTOM_LEFT,
        LV_ALIGN_IN_BOTTOM_MID,
        LV_ALIGN_IN_BOTTOM_RIGHT
    };
    lv_obj_align(app.cont, parent, align[index], mod[index % 3], mod[index / 3]);
}

static void Creat_Tab(lv_obj_t* tabview, AppCont_TypeDef &appcont)
{
    appcont.tab = lv_tabview_add_tab(tabview, appcont.text);
}

static void Creat_Page(lv_obj_t** tabview)
{
    static lv_style_t style_tv_bg;
    lv_style_copy(&style_tv_bg, &lv_style_plain);
    style_tv_bg.body.main_color = LV_COLOR_WHITE;
    style_tv_bg.body.grad_color = LV_COLOR_GRAY;
    
    static lv_style_t style_tv_indic;
    lv_style_copy(&style_tv_indic, &lv_style_pretty);
    style_tv_indic.body.padding.inner = 5;

    /*Create a Tab view object*/
    *tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_set_size(*tabview, page_width, page_height);
    lv_obj_align(*tabview, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    
    lv_tabview_set_style(*tabview, LV_TABVIEW_STYLE_BG, &style_tv_bg);
    lv_tabview_set_style(*tabview, LV_TABVIEW_STYLE_INDIC, &style_tv_indic);
    
    lv_tabview_set_btns_pos(*tabview, LV_TABVIEW_BTNS_POS_BOTTOM);
    //lv_tabview_set_btns_hidden(*tabview, true);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    __ExecuteOnce(Creat_Page(&tabviewHome));
    
    lv_tabview_set_btns_hidden(tabviewHome, false);
    lv_tabview_set_sliding(tabviewHome, true);

    /*creat tab and appcont*/
    __ExecuteOnce(__LoopExecute(Creat_Tab(tabviewHome, AppCont_Grp[i]), __Sizeof(AppCont_Grp)));
    
    /*set tab to home*/
    __ExecuteOnce(lv_tabview_set_tab_act(tabviewHome, tabHomeIndex, LV_ANIM_ON));
    
    /*put apps to appcont0*/
    __LoopExecute(Creat_APP(APP_Grp[i], AppCont_Grp[1].tab, i), __Sizeof(APP_Grp));
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_tabview_set_btns_hidden(tabviewHome, true);
    lv_tabview_set_sliding(tabviewHome, false);
    /*del apps cont */
    __LoopExecute(lv_obj_del_safe(&APP_Grp[i].cont), __Sizeof(APP_Grp));
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnHome)
        {
            lv_tabview_set_tab_act(tabviewHome, tabHomeIndex, LV_ANIM_ON);
        }
    }
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
