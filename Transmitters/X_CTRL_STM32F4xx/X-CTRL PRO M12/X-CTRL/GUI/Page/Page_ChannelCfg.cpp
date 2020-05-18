#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/lv_settings.h"
#include "Communication/ComPrivate.h"
#include "BSP/IMU_Private.h"

static lv_obj_t * appWindow;

static lv_settings menu;

enum item_index{
    IIDX_JS_LX,
    IIDX_JS_LY,
    IIDX_JS_RX,
    IIDX_JS_RY,
    
    IIDX_SW_E,
    IIDX_SW_F,
    IIDX_SW_G,
    IIDX_SW_H,
    
    IIDX_IMU_Pitch,
    IIDX_IMU_Role,
    IIDX_IMU_Yaw,
    IIDX_IMU_dPitch,
    IIDX_IMU_dRoll,
    IIDX_IMU_dYaw,
    IIDX_CH_REV,
    IIDX_SW_Filter,
    IIDX_MAX
};

#define IIDX_CH_MAX (IIDX_CH_REV)

static const char* ChannelStr = 
    "---\n"
    "Channel 0\n"
    "Channel 1\n"
    "Channel 2\n"
    "Channel 3\n"
    "Channel 4\n"
    "Channel 5\n"
    "Channel 6\n"
    "Channel 7"
;

static lv_settings::item_t item_grp[IIDX_MAX] =
{
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    {.type = menu.TYPE_DDLIST,},
    
    {.type = menu.TYPE_BTN, .name = "Channel reverse",.value = "Config",             .user_data.int32 = PAGE_ChannelRevCfg},
    
    {.type = menu.TYPE_SW,  .name = "Filter",         .value = "Joystick lowpass filter",  .user_data.ptr = &CTRL.State.JostickFilter},
};

static void Menu_UpdateChannel()
{
    for(int i = 0; i < IIDX_CH_MAX; i++)
    {
        lv_settings::item_t * item = &item_grp[i];
        lv_obj_t * ddlist = item->obj;
        
        int ch = RCX::ChannelGetAttachedIndex((int16_t*)item->user_data.ptr);
        item->state = ch + 1;
        lv_ddlist_set_selected(ddlist, item->state);
    }
    Com_DevChannelUpdate();
}

static void Menu_EventHnadler(lv_obj_t * obj, lv_event_t event)
{
    /*Get the caller item*/
    lv_settings::item_t * act_item = (lv_settings::item_t *)lv_event_get_data();
#define IS_ITEM(item_name) (strcmp((item_name), act_item->name) == 0)
    
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        if(IS_ITEM("Filter"))
            return;
        
        int new_ch = act_item->state - 1;
        if(new_ch >= 0)
        {
            RCX::ChannelSetAttach(new_ch, (int16_t*)act_item->user_data.ptr);
        }
        else
        {
            int ch = RCX::ChannelGetAttachedIndex((int16_t*)act_item->user_data.ptr);
            if(ch >= 0)
                RCX::ChannelSetAttach(ch, NULL);
        }
        Menu_UpdateChannel();
    }
    else if(event == LV_EVENT_CLICKED)
    {
        if(IS_ITEM("Channel reverse"))
        {
            uint8_t pageID = act_item->user_data.int32;
            if(pageID != PAGE_NONE)
            {
                page.PagePush(pageID);
            }
        }
    }
}

static void Menu_Init()
{
    menu.create(appWindow, Menu_EventHnadler);
    for(int i = 0; i < __Sizeof(item_grp); i++)
    {
        lv_settings::item_t * item = &item_grp[i];
        
        if(i < IIDX_CH_MAX)
        {
            item->name = Com_DevGetName(i + 1);
            item->user_data.ptr = Com_DevGetPtr(i + 1);
            
            int index = RCX::ChannelGetAttachedIndex((int16_t*)item->user_data.ptr);
            item->value = ChannelStr;
            item->state = index + 1;
        }
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
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    Menu_AnimOpen(&menu, false);
    menu.del();
    lv_obj_clean(appWindow);
    if(!EEPROM_SaveAll())
    {
        Audio_PlayMusic(MC_Type::MC_UnstableConnect);
    }
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* btn)
{
    if(btn == &btBACK)
    {
        if(event == ButtonEvent_Type::EVENT_ButtonPress)
        {
            if(!menu.back())
            {
                page.PagePop();
            }
        }
    }

    if(btn == &btOK)
    {
        if(event == ButtonEvent_Type::EVENT_ButtonPress)
        {
            menu.click();
        }
        else if(event == ButtonEvent_Type::EVENT_ButtonLongPressed)
        {
            Com_ChannelSetDefault();
            Menu_UpdateChannel();
        }
    }

    if(event == ButtonEvent_Type::EVENT_ButtonPress || event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat)
    {
        lv_coord_t dist = (event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat) ? 5 : 1;
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
void PageRegister_ChannelCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
