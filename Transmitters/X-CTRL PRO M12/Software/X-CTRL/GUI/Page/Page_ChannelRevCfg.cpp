#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/lv_settings.h"
#include "Communication/ComPrivate.h"

static lv_obj_t * appWindow;

static lv_settings menu;

enum item_index{
    IIDX_SW_CH0,
    IIDX_SW_CH1,
    IIDX_SW_CH2,
    IIDX_SW_CH3,
    IIDX_SW_CH4,
    IIDX_SW_CH5,
    IIDX_SW_CH6,
    IIDX_SW_CH7,
    IIDX_MAX
};

static lv_settings::item_t item_grp[IIDX_MAX] =
{
    {.type = menu.TYPE_SW,    .name = "Channel 0",},
    {.type = menu.TYPE_SW,    .name = "Channel 1",},
    {.type = menu.TYPE_SW,    .name = "Channel 2",},
    {.type = menu.TYPE_SW,    .name = "Channel 3",},
    {.type = menu.TYPE_SW,    .name = "Channel 4",},
    {.type = menu.TYPE_SW,    .name = "Channel 5",},
    {.type = menu.TYPE_SW,    .name = "Channel 6",},
    {.type = menu.TYPE_SW,    .name = "Channel 7",}
};

static char ChannelDataStr[RCX_CHANNEL_NUM][10];
static lv_task_t * taskCh;

static void Task_ChannelDataUpdate(lv_task_t * task)
{
    for(int i = 0; i < __Sizeof(item_grp); i++)
    {
        RCX::ChannelUpdate();
        snprintf(ChannelDataStr[i], sizeof(ChannelDataStr[i]), "% 4d", RCX::ChannelRead(i));
        menu.refr(&item_grp[i]);
    }
}

static void Menu_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    /*Get the caller item*/
    lv_settings::item_t * act_item = (lv_settings::item_t *)lv_event_get_data();
#define IS_ITEM(item_name) (strcmp((item_name), act_item->name) == 0)
    
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        int ch = act_item->name[8] - '0';
        RCX::ChannelSetReverse(ch, act_item->state);
    }
}

static void Menu_Init()
{
    menu.create(appWindow, Menu_EventHandler);
    for(int i = 0; i < __Sizeof(item_grp); i++)
    {
        lv_settings::item_t * item = &item_grp[i];
        item->user_data.ptr = &CTRL.CH_Config->Reverse[i];
        item->state = RCX::ChannelGetReverse(i);
        item->value = ChannelDataStr[i];
        menu.add(item);
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
    Menu_Init();
    Menu_AnimOpen(&menu, true);
    
    taskCh = lv_task_create(Task_ChannelDataUpdate, 100, LV_TASK_PRIO_MID, 0);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    Menu_AnimOpen(&menu, false);
    lv_task_del(taskCh);
    menu.del();
    lv_obj_clean(appWindow);
}

/**
  * @brief  页面事件
  * @param  btn:发出事件的按键
  * @param  event:事件编号
  * @retval 无
  */
static void Event(void* btn, int event)
{
    if(btn == &btBACK)
    {
        if(event == ButtonEvent::EVENT_ButtonPress)
        {
            if(!menu.back())
            {
                page.PagePop();
            }
        }
    }

    if(btn == &btOK)
    {
        if(event == ButtonEvent::EVENT_ButtonPress)
        {
            menu.click();
        }
    }

    if(event == ButtonEvent::EVENT_ButtonPress || event == ButtonEvent::EVENT_ButtonLongPressRepeat)
    {
        lv_coord_t dist = (event == ButtonEvent::EVENT_ButtonLongPressRepeat) ? 5 : 1;
        if(btn == &btUP)
        {
            menu.move(-dist);
        }
        if(btn == &btDOWN)
        {
            menu.move(+dist);
        }
        if(btn == &btUPL)
        {
            menu.scroll(-20);
        }
        if( btn == &btDOWNL)
        {
            menu.scroll(+20);
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_ChannelRevCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
