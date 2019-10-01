#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"

static lv_obj_t * mbox1;

static void mbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        int index = lv_mbox_get_active_btn(obj);
        if(index == 0)
        {
            Power_Shutdown();
        }
        else if(index == 1)
        {
            NVIC_SystemReset();
        }
        lv_mbox_start_auto_close(obj, 20);
    }
}

static void Creat_ShutdownMessageBox(void)
{
    static const char * btns[] ={"Shutdown", "Reboot", "Cancel",""};

    mbox1 = lv_mbox_create(lv_scr_act(), NULL);
    
    lv_mbox_set_text(mbox1, "Shutdown / Reboot?");
    lv_mbox_add_btns(mbox1, btns);
    lv_obj_set_size(mbox1, 250, 200);
    lv_obj_set_event_cb(mbox1, mbox_event_handler);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
    
//    lv_mbox_set_style(mbox1, LV_MBOX_STYLE_BG, &lv_style_pretty);
//    //lv_mbox_set_style(mbox1, LV_MBOX_STYLE_BTN_BG, &lv_style_pretty_color);
//    lv_mbox_set_style(mbox1, LV_MBOX_STYLE_BTN_REL, &lv_style_pretty_color);
//    lv_mbox_set_style(mbox1, LV_MBOX_STYLE_BTN_PR, &lv_style_pretty);
}

typedef struct{
    const void* symbol;
    const char* text;
    Func_Type type;
    int param;
}ListBtn_TypeDef;

static ListBtn_TypeDef ListBtn_Grp[] = {
    {LV_SYMBOL_IMAGE,     " Display", TYPE_PageJump, PAGE_SetDisplay},
    {LV_SYMBOL_BATTERY_3, " Battery", TYPE_PageJump, PAGE_BattInfo},
    {LV_SYMBOL_WIFI,      " Radio"},
    {LV_SYMBOL_AUDIO,     " Audio"},
    {LV_SYMBOL_DIRECTORY, " Files"},
    {LV_SYMBOL_POWER,     " Power",   TYPE_FuncCall, (int)Creat_ShutdownMessageBox}
};

static lv_obj_t * listItems;
static lv_obj_t * listItems_btn;

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    int index = lv_list_get_btn_index(listItems, obj);
    if(event == LV_EVENT_CLICKED)
    {
        if(ListBtn_Grp[index].type == TYPE_PageJump)
        {
            page.PageChangeTo(ListBtn_Grp[index].param);
        }
        else if(ListBtn_Grp[index].type == TYPE_FuncCall)
        {
            typedef void(*void_func_t)(void);
            ((void_func_t)ListBtn_Grp[index].param)();
        }
    }
}

static void Creat_ListBtn(lv_obj_t* parent,lv_obj_t** list_btn)
{
    /*Add buttons to the list*/
    for(int i = 0; i < __Sizeof(ListBtn_Grp); i++)
    {
        *list_btn = lv_list_add_btn(parent, ListBtn_Grp[i].symbol, ListBtn_Grp[i].text);
        lv_obj_set_event_cb(*list_btn, event_handler);
    }
}

static void Creat_List(lv_obj_t** list)
{
//    static lv_style_t style_btn_pr;
//    static lv_style_t style_btn_rel;
//    lv_style_copy(&style_btn_pr, &lv_style_plain);
//    style_btn_pr.body.main_color = LV_COLOR_GRAY;
//    style_btn_pr.body.grad_color = LV_COLOR_GRAY;
//    style_btn_pr.body.padding.top = 30;
//    style_btn_pr.body.padding.bottom = 30;
//    
//    lv_style_copy(&style_btn_rel, &style_btn_pr);
//    style_btn_pr.body.main_color = lv_color_hex(0x487fb7);
//    style_btn_pr.body.grad_color = lv_color_hex(0x487fb7);
    
    /*Create a list*/
    *list = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(*list, lv_obj_get_width(barStatus), page_height);
    lv_obj_align(*list, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_list_set_edge_flash(*list, true);
//    lv_list_set_style(*list, LV_LIST_STYLE_BTN_PR, &style_btn_pr);
//    lv_list_set_style(*list, LV_LIST_STYLE_BTN_REL, &style_btn_rel);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    Creat_List(&listItems);
    Creat_ListBtn(listItems, &listItems_btn);
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
    lv_obj_del_safe(&listItems);
    lv_obj_del_safe(&mbox1);
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
            page.PageChangeTo(PAGE_Home);
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Settings(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
