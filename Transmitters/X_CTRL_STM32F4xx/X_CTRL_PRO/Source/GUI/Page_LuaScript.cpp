#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "LuaScript.h"
#include "Module.h"

static void write_create(lv_obj_t * parent);
static void luaoutput_creat(lv_obj_t * parent);
static void TextAreaEvent_Handler(lv_obj_t * text_area, lv_event_t event);
static void KeyboardEvent_Handler(lv_obj_t * kb, lv_event_t event);

static lv_obj_t * ta_input;
static lv_obj_t * ta_output;
static lv_obj_t * tv;
static lv_obj_t * keyboard;

void PageCreat_LuaScript()
{   
    tv = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(tv, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_size(tv, APP_WIN_WIDTH, APP_WIN_HEIGHT);

    lv_obj_t * tab1 = lv_tabview_add_tab(tv, LV_SYMBOL_EDIT);
    lv_obj_t * tab2 = lv_tabview_add_tab(tv, LV_SYMBOL_UPLOAD);

    write_create(tab1);
    luaoutput_creat(tab2);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static const char *luaCode = "";
//"while 1 do\n"
//"  motor(1,100)\n"
//"  print(millis())\n"
//"  delay(1000)\n"
//"end"
//;

void LuaCodeSet(const char* code)
{
    luaCode = code;
}

static void write_create(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_DRAG);

    /*text area*/
    ta_input = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta_input, lv_page_get_scrl_width(parent), lv_obj_get_height(parent));
    lv_ta_set_text(ta_input, luaCode);
    lv_obj_set_event_cb(ta_input, TextAreaEvent_Handler);
    lv_ta_set_text_sel(ta_input, true);
}

static void luaoutput_creat(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_DRAG);

    /*text area*/
    ta_output = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta_output, lv_page_get_scrl_width(parent), lv_obj_get_height(parent));
    lv_ta_set_cursor_type(ta_output, LV_CURSOR_NONE);
    lv_ta_set_text(ta_output, "");
}

static void TextAreaEvent_Handler(lv_obj_t * text_area, lv_event_t event)
{
    /*Text area is on the scrollable part of the page but we need the page itself*/
    lv_obj_t * parent = lv_obj_get_parent(lv_obj_get_parent(ta_input));

    if(event == LV_EVENT_CLICKED)
    {
        Keyboard_Activate(&keyboard, true, parent, ta_input, KeyboardEvent_Handler);
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
static void KeyboardEvent_Handler(lv_obj_t * kb, lv_event_t event)
{
    lv_kb_def_event_cb(kb, event);
    const char * txt = lv_btnm_get_active_btn_text(kb);
    
    if(event == LV_EVENT_PRESSED)
    {
        Motor_Vibrate(1.0f, 20);
    }
    
    if(event == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        if(strcmp(txt, "Bksp") == 0)
        {
            lv_ta_set_text(ta_input, "");
        }
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
        Keyboard_Activate(&keyboard, false, NULL, NULL, NULL);
    }
    
    if(event == LV_EVENT_DELETE)
    {
        keyboard = NULL;
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    __ExecuteOnce(PageCreat_LuaScript());
    lv_obj_set_hidden(tv, false);
    lv_ta_set_text(ta_input, luaCode);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    luaScript.end();
    lv_obj_set_hidden(tv, true);
    //lv_obj_del_safe(&tv);
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
            if(keyboard)
            {
                lv_event_send(keyboard, LV_EVENT_CANCEL, NULL);
            }
            else
            {
                page.PagePop();
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

