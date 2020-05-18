#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/lv_settings.h"
#include "Communication/ComPrivate.h"

static lv_obj_t * appWindow;

static lv_settings menu;

/*string buffer*/
static char FrequencyStr[10] = "2400MHz";
static char AddrStr[5][10];

enum item_index{
    IIDX_Passback,
    IIDX_FHSS,
    IIDX_Handshake,
    IIDX_SignWarn,
    IIDX_Baudrate,
    IIDX_Frequency,
    IIDX_Addr0,
    IIDX_Addr1,
    IIDX_Addr2,
    IIDX_Addr3,
    IIDX_Addr4,
    IIDX_MAX
};

static lv_settings::item_t item_grp[IIDX_MAX] =
{
    {.type = menu.TYPE_SW,     .name = "Passback",    .value = "Check passback data",   .user_data.ptr = &CTRL.State.Passback},
    {.type = menu.TYPE_SW,     .name = "FHSS",        .value = "Use FHSS mode",         .user_data.ptr = &CTRL.State.FHSS},
    {.type = menu.TYPE_SW,     .name = "Handshake",   .value = "Handshake to slave",    .user_data.ptr = &CTRL.State.Handshake},
    {.type = menu.TYPE_SW,     .name = "Signal warn", .value = "Signal low warn",       .user_data.ptr = &CTRL.State.SignWarn},
    {.type = menu.TYPE_DDLIST, .name = "Baudrate",    .value = "250Kbps\n1Mbps\n2Mbps", .user_data.ptr = &CTRL.RF_Config.Speed},
    {.type = menu.TYPE_SLIDER, .name = "Frequency",   .value = FrequencyStr,            .user_data.ptr = &CTRL.RF_Config.Freq},
    {.type = menu.TYPE_NUMSET, .name = "Address[0]",     .value = AddrStr[0]},
    {.type = menu.TYPE_NUMSET, .name = "Address[1]",     .value = AddrStr[1]},
    {.type = menu.TYPE_NUMSET, .name = "Address[2]",     .value = AddrStr[2]},
    {.type = menu.TYPE_NUMSET, .name = "Address[3]",     .value = AddrStr[3]},
    {.type = menu.TYPE_NUMSET, .name = "Address[4]",     .value = AddrStr[4]},
};



static void Menu_EventHnadler(lv_obj_t * obj, lv_event_t event)
{
    /*Get the caller item*/
    lv_settings::item_t * act_item = (lv_settings::item_t *)lv_event_get_data();
#define IS_ITEM(item_name) (strcmp((item_name), act_item->name) == 0)
    
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        if(IS_ITEM("Baudrate"))
        {
            CTRL.RF_Config.Speed = act_item->state;
            nrf.SetSpeed(CTRL.RF_Config.Speed);
        }
        else if(IS_ITEM("Frequency"))
        {
            CTRL.RF_Config.Freq = act_item->state;
            snprintf(FrequencyStr, sizeof(FrequencyStr), "%04dMHz", CTRL.RF_Config.Freq + 2400);
            nrf.SetFreqency(CTRL.RF_Config.Freq);
        }
        else if(String(act_item->name).startsWith("Address["))
        {
            uint8_t index = act_item->name[8] - '0';
            
            if(index > 4)
                return;
            
            __LimitValue(act_item->state, 0, 0xFF);
            snprintf(AddrStr[index], sizeof(AddrStr[index]), "0x%02x", act_item->state);
            CTRL.RF_Config.Addr[index] = act_item->state;
            nrf.SetAddress(CTRL.RF_Config.Addr);
        }
    }
}

static void Menu_Init()
{
    menu.create(appWindow, Menu_EventHnadler);
    menu.add(item_grp, __Sizeof(item_grp));
    
    /*Baudrate*/
    item_grp[IIDX_Baudrate].state = CTRL.RF_Config.Speed;
    lv_obj_t * ddlist = item_grp[IIDX_Baudrate].obj;
    lv_ddlist_set_selected(ddlist, item_grp[IIDX_Baudrate].state);
    //menu.refr(&item_grp[IIDX_Baudrate]);/*别用这个*/

    /*Frequency slider*/
    lv_obj_t * slider = item_grp[IIDX_Frequency].obj;
    lv_slider_set_range(slider, 0, 125);
    item_grp[IIDX_Frequency].state = nrf.GetFreqency();
    snprintf(FrequencyStr, sizeof(FrequencyStr), "%04dMHz", item_grp[IIDX_Frequency].state + 2400);
    menu.refr(&item_grp[IIDX_Frequency]);
    
    /*ADDR*/
    for(int i = 0; i < 5; i++)
    {
        uint8_t addr = nrf.GetAddress(i);
        CTRL.RF_Config.Addr[i] = addr;
        item_grp[IIDX_Addr0 + i].state = addr;
        snprintf(AddrStr[i], sizeof(AddrStr[i]), "0x%02x", addr);
        menu.refr(&item_grp[IIDX_Addr0 + i]);
    }
}

void Menu_AnimOpen(lv_settings * menu, bool open)
{
    lv_obj_t * cont = menu->act_cont;
    lv_opa_t opa_target = LV_OPA_COVER;
    lv_coord_t y_target = lv_obj_get_y(cont);
    if(open)
    {
        lv_obj_set_opa_scale_enable(cont, true);
        
        lv_obj_set_opa_scale(cont, LV_OPA_TRANSP);
        lv_obj_set_y(cont, y_target + lv_obj_get_height(cont));
    }
    else
    {
        opa_target = LV_OPA_TRANSP;
        y_target = lv_obj_get_y(cont) + lv_obj_get_height(cont);
    }
    LV_OBJ_ADD_ANIM(cont, opa_scale, opa_target, LV_ANIM_TIME_DEFAULT);
    LV_OBJ_ADD_ANIM(cont, y, y_target, LV_ANIM_TIME_DEFAULT);
    Page_Delay(LV_ANIM_TIME_DEFAULT);
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
        if(event == ButtonEvent_Type::EVENT_ButtonPress || event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat)
        {
            menu.click();
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
void PageRegister_RadioCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
