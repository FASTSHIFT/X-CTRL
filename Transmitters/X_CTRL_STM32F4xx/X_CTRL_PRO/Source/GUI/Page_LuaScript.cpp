#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "LuaScript.h"

static void write_create(lv_obj_t * parent);
static void luaoutput_creat(lv_obj_t * parent);
static void text_area_event_handler(lv_obj_t * text_area, lv_event_t event);
static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event);
#if LV_USE_ANIMATION
static void kb_hide_anim_end(lv_anim_t * a);
#endif

static lv_obj_t * ta_input;
static lv_obj_t * ta_output;
static lv_obj_t * kb;
static lv_obj_t * tv;

static lv_style_t style_kb;
static lv_style_t style_kb_rel;
static lv_style_t style_kb_pr;

void PageCreat_LuaScript()
{
    //lv_theme_set_current(lv_theme_night_init(100, NULL));
    
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = page_height;

    static lv_style_t style_tv_btn_bg;
    lv_style_copy(&style_tv_btn_bg, &lv_style_plain);
    style_tv_btn_bg.body.main_color = lv_color_hex(0x487fb7);
    style_tv_btn_bg.body.grad_color = lv_color_hex(0x487fb7);
    style_tv_btn_bg.body.padding.top = 0;
    style_tv_btn_bg.body.padding.bottom = 0;

    static lv_style_t style_tv_btn_rel;
    lv_style_copy(&style_tv_btn_rel, &lv_style_btn_rel);
    style_tv_btn_rel.body.opa = LV_OPA_TRANSP;
    style_tv_btn_rel.body.border.width = 0;

    static lv_style_t style_tv_btn_pr;
    lv_style_copy(&style_tv_btn_pr, &lv_style_btn_pr);
    style_tv_btn_pr.body.radius = 0;
    style_tv_btn_pr.body.opa = LV_OPA_50;
    style_tv_btn_pr.body.main_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.grad_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.border.width = 0;
    style_tv_btn_pr.text.color = LV_COLOR_GRAY;

    tv = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(tv, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_size(tv, hres, vres);

    lv_obj_t * tab1 = lv_tabview_add_tab(tv, "Input");
    lv_obj_t * tab2 = lv_tabview_add_tab(tv, "Output");

#if LV_DEMO_WALLPAPER == 0
    /*Blue bg instead of wallpaper*/
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BG, &style_tv_btn_bg);
#endif
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_BG, &style_tv_btn_bg);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_INDIC, &lv_style_plain);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_REL, &style_tv_btn_rel);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_PR, &style_tv_btn_pr);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_REL, &style_tv_btn_rel);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_PR, &style_tv_btn_pr);

    write_create(tab1);
    luaoutput_creat(tab2);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static const char luaCode[] =
"while 1 do\n"
"  motor(1,100)\n"
"  print(millis())\n"
"  delay(1000)\n"
"end"
;

static void write_create(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_DRAG);

    /*text area*/
    static lv_style_t style_ta;
    lv_style_copy(&style_ta, &lv_style_pretty);
    style_ta.body.opa = LV_OPA_30;
    style_ta.body.radius = 0;
    style_ta.text.color = lv_color_hex3(0x222);

    ta_input = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta_input, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) / 2);
    lv_ta_set_style(ta_input, LV_TA_STYLE_BG, &style_ta);
    lv_ta_set_text(ta_input, luaCode);
    lv_obj_set_event_cb(ta_input, text_area_event_handler);
    lv_style_copy(&style_kb, &lv_style_plain);
    lv_ta_set_text_sel(ta_input, true);

    /*keyboard*/
    #define MainColor LV_COLOR_BLACK
    #define GradColor LV_COLOR_BLACK
    style_kb.body.opa = LV_OPA_COVER;
    style_kb.body.main_color = MainColor;//lv_color_hex3(0x333);
    style_kb.body.grad_color = GradColor;//lv_color_hex3(0x333);
    style_kb.body.padding.left = 0;
    style_kb.body.padding.right = 0;
    style_kb.body.padding.top = 0;
    style_kb.body.padding.bottom = 0;
    style_kb.body.padding.inner = 0;

    lv_style_copy(&style_kb_rel, &lv_style_plain);
    style_kb_rel.body.opa = LV_OPA_TRANSP;
    style_kb_rel.body.radius = 0;
    style_kb_rel.body.border.width = 1;
    style_kb_rel.body.border.color = LV_COLOR_SILVER;
    style_kb_rel.body.border.opa = LV_OPA_50;
    style_kb_rel.body.main_color = MainColor;//lv_color_hex3(0x333);    /*Recommended if LV_VDB_SIZE == 0 and bpp > 1 fonts are used*/
    style_kb_rel.body.grad_color = GradColor;//lv_color_hex3(0x333);
    style_kb_rel.text.color = LV_COLOR_WHITE;

    lv_style_copy(&style_kb_pr, &lv_style_btn_pr);
    style_kb_pr.body.radius = 0;
    style_kb_pr.body.opa = LV_OPA_50;
    style_kb_pr.body.main_color = LV_COLOR_WHITE;
    style_kb_pr.body.grad_color = LV_COLOR_WHITE;
    style_kb_pr.body.border.width = 1;
    style_kb_pr.body.border.color = LV_COLOR_SILVER;
}

static void luaoutput_creat(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_DRAG);

    /*text area*/
    static lv_style_t style_ta;
    lv_style_copy(&style_ta, &lv_style_pretty);
    style_ta.body.opa = LV_OPA_30;
    style_ta.body.radius = 0;
    style_ta.text.color = lv_color_hex3(0x222);

    ta_output = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta_output, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) * 0.8f);
    lv_ta_set_style(ta_output, LV_TA_STYLE_BG, &style_ta);
    lv_ta_set_cursor_type(ta_output, LV_CURSOR_NONE);
    lv_ta_set_text(ta_output, "");
}

static void text_area_event_handler(lv_obj_t * text_area, lv_event_t event)
{
    (void) text_area;    /*Unused*/

    /*Text area is on the scrollable part of the page but we need the page itself*/
    lv_obj_t * parent = lv_obj_get_parent(lv_obj_get_parent(ta_input));

    if(event == LV_EVENT_CLICKED) {
        if(kb == NULL) {
            kb = lv_kb_create(parent, NULL);
            lv_obj_set_size(kb, lv_obj_get_width_fit(parent), lv_obj_get_height_fit(parent) / 2);
            lv_obj_align(kb, ta_input, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
            lv_kb_set_ta(kb, ta_input);
            lv_kb_set_style(kb, LV_KB_STYLE_BG, &style_kb);
            lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &style_kb_rel);
            lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &style_kb_pr);
            lv_obj_set_event_cb(kb, keyboard_event_cb);

#if LV_USE_ANIMATION
            lv_anim_t a;
            a.var = kb;
            a.start = LV_VER_RES;
            a.end = lv_obj_get_y(kb);
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
            a.path_cb = lv_anim_path_linear;
            a.ready_cb = NULL;
            a.act_time = 0;
            a.time = 300;
            a.playback = 0;
            a.playback_pause = 0;
            a.repeat = 0;
            a.repeat_pause = 0;
            lv_anim_create(&a);
#endif
        }
    }

}

void LuaPrintCallback(const char* s)
{
    if(!ta_output)
        return;
    
    lv_ta_add_text(ta_output, s);
    
    if(lv_ta_get_cursor_pos(ta_output) > 200)
    {
        lv_ta_set_text(ta_output, "");
    }
}

/**
 * Called when the close or ok button is pressed on the keyboard
 * @param keyboard pointer to the keyboard
 * @return
 */
static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event)
{
    (void) keyboard;    /*Unused*/

    lv_kb_def_event_cb(keyboard, event);
    
    if(event == LV_EVENT_PRESSED)
    {
        MotorVibrate(1.0f, 20);
    }
    
    if(event == LV_EVENT_APPLY)
    {
        luaScript.registerStrOutput(LuaPrintCallback);
        LuaScriptStart(lv_ta_get_text(ta_input));
        lv_tabview_set_tab_act(tv, 1, true);
        lv_ta_add_text(ta_output, "\n> ");
    }
    
    if(event == LV_EVENT_CANCEL)
    {
        luaScript.end();
#if LV_USE_ANIMATION
        lv_anim_t a;
        a.var = kb;
        a.start = lv_obj_get_y(kb);
        a.end = LV_VER_RES;
        a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
        a.path_cb = lv_anim_path_linear;
        a.ready_cb = kb_hide_anim_end;
        a.act_time = 0;
        a.time = 300;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);
#else
        lv_obj_del(kb);
        kb = NULL;
#endif
    }
    
    if(event == LV_EVENT_LONG_PRESSED)
    {
        lv_ta_set_text(ta_input, "");
    }
}

#if LV_USE_ANIMATION
static void kb_hide_anim_end(lv_anim_t * a)
{
    lv_obj_del((lv_obj_t*)a->var);
    kb = NULL;
}
#endif

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    PageCreat_LuaScript();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    luaScript.end();

    lv_obj_del(tv);
    ta_input = ta_output = tv = kb = NULL;
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
        if(btn == btnBack)
        {
            if(kb)
            {
                lv_event_send(kb, LV_EVENT_CANCEL, NULL);
            }
            else
            {
                page.PageChangeTo(page.LastPage);
            }
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_LuaScript(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}

