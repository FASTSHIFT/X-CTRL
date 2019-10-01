#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "LuaScript.h"
#include "Module.h"

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

static lv_obj_t * btn_clear;

void PageCreat_LuaScript()
{   
    tv = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(tv, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_size(tv, APP_WIN_WIDTH, APP_WIN_HEIGHT);

    lv_obj_t * tab1 = lv_tabview_add_tab(tv, "Input");
    lv_obj_t * tab2 = lv_tabview_add_tab(tv, "Output");

#if LV_DEMO_WALLPAPER == 0
    /*Blue bg instead of wallpaper*/
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BG, &style_tv_btn_bg);
#endif

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

static void btn_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_LONG_PRESSED)
    {
        lv_ta_set_text(ta_input, "");
    }
}

static void write_create(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_DRAG);

    /*text area*/
    ta_input = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta_input, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) / 2);
    lv_ta_set_text(ta_input, luaCode);
    lv_obj_set_event_cb(ta_input, text_area_event_handler);
    lv_ta_set_text_sel(ta_input, true);
    
    /*btn*/
    btn_clear = lv_btn_create(ta_input, NULL);
    lv_obj_set_event_cb(btn_clear, btn_event_handler);
    lv_obj_align(btn_clear, ta_input, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5);
    lv_obj_t * label = lv_label_create(btn_clear, NULL);
    lv_label_set_text(label, "Clear");
}

static void luaoutput_creat(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_DRAG);

    /*text area*/
    ta_output = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta_output, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) * 0.8f);
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

static void LuaPrintCallback(const char* s)
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
        Motor_Vibrate(1.0f, 20);
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
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}

