#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"

static lv_obj_t * mboxPower;

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
        lv_obj_del_async(obj);
        mboxPower = NULL;
    }
    if(event == LV_EVENT_DELETE)
    {
        
    }
}

static void Creat_ShutdownMessageBox(void * param)
{
    lv_obj_t **mbox = (lv_obj_t**)param;
    static const char * btns[] ={"Shutdown", "Reboot", "Cancel",""};

    *mbox = lv_mbox_create(appWindow, NULL);
    
    lv_mbox_set_text(*mbox, "Shutdown / Reboot?");
    lv_mbox_add_btns(*mbox, btns);
    //lv_obj_set_drag(*mbox, true);
    lv_obj_set_size(*mbox, 250, 200);
    lv_obj_set_event_cb(*mbox, mbox_event_handler);
    lv_obj_align(*mbox, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
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
            typedef void(*void_func_t)(void*);
            ((void_func_t)ListBtn_Grp[index].param)(&mboxPower);
        }
    }
}

static void Creat_ListBtn(lv_obj_t* parent,lv_obj_t** list_btn)
{
    /*Add buttons to the list*/
    for(int i = 0; i < __Sizeof(ListBtn_Grp); i++)
    {
        *list_btn = lv_list_add_btn(parent, ListBtn_Grp[i].symbol, ListBtn_Grp[i].text);
//        static lv_style_t style = *lv_obj_get_style(*list_btn);
//        style.text.font = &lv_font_roboto_28;
//        lv_obj_set_style(*list_btn, &style);
        
        lv_btn_set_ink_in_time(*list_btn, 200);
        lv_btn_set_ink_out_time(*list_btn, 200);
        lv_obj_set_event_cb(*list_btn, event_handler);
    }
}

static void Creat_List(lv_obj_t** list)
{
    /*Create a list*/
    *list = lv_list_create(appWindow, NULL);
    lv_obj_set_size(*list, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    lv_obj_align(*list, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_list_set_edge_flash(*list, true);
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
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_del_safe(&listItems);
    lv_obj_del_safe(&mboxPower);
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
            if(mboxPower)
            {
                lv_event_send(mboxPower, LV_EVENT_CLICKED, 0);
            }
            else
            {
                page.PageChangeTo(PAGE_Home);
            }
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
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
