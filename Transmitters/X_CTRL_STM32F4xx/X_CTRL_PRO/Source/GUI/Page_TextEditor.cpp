#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"

static lv_obj_t * topBar;
static lv_obj_t * ta_input;
static lv_obj_t * btnColse;
static lv_obj_t * btnEdit;
static lv_obj_t * btnSave;
static lv_obj_t * labelFileName;
static lv_obj_t * keyboard;
static lv_obj_t * labelEditState;
static bool IsEditting = false;

static void KeyboardEvent_Handler(lv_obj_t * kb, lv_event_t event);

static void BtnEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(obj == btnColse)
        {
            if(keyboard)
            {
                lv_event_send(keyboard, LV_EVENT_CANCEL, NULL);
            }
            else if(IsEditting)
            {
                lv_event_send(btnEdit, LV_EVENT_CLICKED, NULL);
            }
            else
            {
                page.PagePop();
            }
        }
        else if(obj == btnEdit)
        {
            IsEditting = !IsEditting;
            lv_label_set_text(labelEditState, IsEditting ? LV_SYMBOL_OK : LV_SYMBOL_EDIT);
            if(IsEditting)
            {
                lv_ta_set_cursor_type(ta_input, LV_CURSOR_LINE);
                Keyboard_Activate(&keyboard, true, appWindow, ta_input, KeyboardEvent_Handler);
            }
            else
            {
                lv_event_send(keyboard, LV_EVENT_CANCEL, NULL);
                lv_ta_set_cursor_type(ta_input, LV_CURSOR_HIDDEN);
            }
        }
        else if(obj == btnSave)
        {
        }
    }
}

static void Creat_TopBar(const char* filename)
{
    topBar = lv_cont_create(appWindow, NULL);
    lv_obj_set_size(topBar, APP_WIN_WIDTH, 40);
    lv_obj_align(topBar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    
    #define BTN_SIZE (lv_obj_get_height(topBar))
    
    btnColse = lv_btn_create(topBar, NULL);
    lv_btn_set_toggle(btnColse, false);
    lv_obj_set_event_cb(btnColse, BtnEvent_Handler);
    lv_obj_set_size(btnColse, BTN_SIZE, BTN_SIZE);
    lv_obj_align(btnColse, topBar, LV_ALIGN_IN_LEFT_MID, 0, 0);
    lv_obj_t * labelColse = lv_label_create(btnColse, NULL);
    lv_label_set_text(labelColse, LV_SYMBOL_CLOSE);
    lv_obj_align(labelColse, NULL, LV_ALIGN_CENTER, 0, 0);
    
    labelFileName = lv_label_create(topBar, NULL);
    lv_label_set_text(labelFileName, filename);
    lv_obj_align(labelFileName, btnColse, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    
    btnSave = lv_btn_create(topBar, NULL);
    lv_btn_set_toggle(btnSave, false);
    lv_obj_set_event_cb(btnSave, BtnEvent_Handler);
    lv_obj_set_size(btnSave, BTN_SIZE, BTN_SIZE);
    lv_obj_align(btnSave, topBar, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_obj_t * labelSave = lv_label_create(btnSave, NULL);
    lv_label_set_text(labelSave, LV_SYMBOL_SAVE);
    lv_obj_align(labelSave, NULL, LV_ALIGN_CENTER, 0, 0);
    
    btnEdit = lv_btn_create(topBar, NULL);
    lv_btn_set_toggle(btnEdit, false);
    lv_obj_set_event_cb(btnEdit, BtnEvent_Handler);
    lv_obj_set_size(btnEdit, BTN_SIZE, BTN_SIZE);
    lv_obj_align(btnEdit, btnSave, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    labelEditState = lv_label_create(btnEdit, NULL);
    lv_label_set_text(labelEditState, LV_SYMBOL_EDIT);
    lv_obj_align(labelEditState, NULL, LV_ALIGN_CENTER, 0, 0);
    
    static lv_style_t style_bar, style_btn = *lv_obj_get_style(btnEdit);
    lv_style_copy(&style_bar, &lv_style_plain_color);
    style_bar.body.main_color = style_btn.body.main_color;
    style_bar.body.grad_color = style_btn.body.grad_color;
    style_bar.body.opa = LV_OPA_COVER;
    lv_obj_set_style(topBar, &style_bar);
}

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
    }
    
    if(event == LV_EVENT_CANCEL)
    {
        Keyboard_Activate(&keyboard, false, NULL, NULL, NULL);
    }
    
    if(event == LV_EVENT_DELETE)
    {
        keyboard = NULL;
    }
}

static void Creat_TextArea()
{
    /*text area*/
    ta_input = lv_ta_create(appWindow, NULL);
    lv_obj_set_size(ta_input, APP_WIN_WIDTH, APP_WIN_HEIGHT - lv_obj_get_height(topBar));
    lv_obj_align(ta_input, topBar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_ta_set_text(ta_input, "");
    lv_ta_set_text_sel(ta_input, true);
    lv_ta_set_cursor_type(ta_input, LV_CURSOR_HIDDEN);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    Creat_TopBar("test.txt");
    Creat_TextArea();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_del_safe(&topBar);
    lv_obj_del_safe(&ta_input);
    lv_obj_del_safe(&keyboard);
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
            else if(IsEditting)
            {
                lv_event_send(btnColse, LV_EVENT_CLICKED, NULL);
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
void PageRegister_TextEditor(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
