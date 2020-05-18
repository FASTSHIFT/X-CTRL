#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/lv_settings.h"
#include "BSP/IMU_Private.h"

static lv_obj_t * appWindow;

static lv_settings menu;

static char AxisLimitStr[3][20];

enum item_index{
    IIDX_SW_GYRO,
    IIDX_BTN_Calibrate,
    IIDX_Pitch,
    IIDX_Roll,
    IIDX_Yaw,
    IIDX_MAX
};

static lv_settings::item_t item_grp[IIDX_MAX] =
{
    {.type = menu.TYPE_SW,     .name = "Gyroscope",    .value = "Use gyroscope",    .user_data.ptr = &CTRL.State.IMU},
    {.type = menu.TYPE_BTN,    .name = "Calibrate",    .value = "Start"},
    {.type = menu.TYPE_NUMSET, .name = "Pitch/Limit",  .value = AxisLimitStr[0],    .user_data.ptr = &IMU_Axis.Pitch.Limit},
    {.type = menu.TYPE_NUMSET, .name = "Roll/Limit",   .value = AxisLimitStr[1],    .user_data.ptr = &IMU_Axis.Roll.Limit},
    {.type = menu.TYPE_NUMSET, .name = "Yaw/Limit",    .value = AxisLimitStr[2],    .user_data.ptr = &IMU_Axis.Yaw.Limit},
};

static lv_task_t * taskIMU_Update;

static void Task_IMU_Update(lv_task_t * task)
{
    snprintf(AxisLimitStr[0], sizeof(AxisLimitStr[0]), "%0.1f/+-%d"LV_SYMBOL_DEGREE_SIGN, IMU_Axis.Pitch.Angle, IMU_Axis.Pitch.Limit);
    menu.refr(&item_grp[IIDX_Pitch]);
    
    snprintf(AxisLimitStr[1], sizeof(AxisLimitStr[1]), "%0.1f/+-%d"LV_SYMBOL_DEGREE_SIGN, IMU_Axis.Roll.Angle, IMU_Axis.Roll.Limit);
    menu.refr(&item_grp[IIDX_Roll]);
    
    snprintf(AxisLimitStr[2], sizeof(AxisLimitStr[2]), "%0.1f/+-%d"LV_SYMBOL_DEGREE_SIGN, IMU_Axis.Yaw.Angle, IMU_Axis.Yaw.Limit);
    menu.refr(&item_grp[IIDX_Yaw]);
}

static void Menu_EventHnadler(lv_obj_t * obj, lv_event_t event)
{
    /*Get the caller item*/
    lv_settings::item_t * act_item = (lv_settings::item_t *)lv_event_get_data();
#define IS_ITEM(item_name) (strcmp((item_name), act_item->name) == 0)
    
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        String name = String(act_item->name);
        if(name.endsWith("Limit"))
        {
            if(name.startsWith("Yaw"))
            {
                __LimitValue(act_item->state, 1, 180);
            }
            else
            {
                __LimitValue(act_item->state, 1, 90);
            }
            *(int16_t*)act_item->user_data.ptr = act_item->state;
        }
    }
    else if(event == LV_EVENT_CLICKED)
    {
        if(IS_ITEM("Calibrate"))
        {
            IMU_CalibrateStart();
        }
    }
}

static void Menu_Init()
{
    menu.create(appWindow, Menu_EventHnadler);
    for(int i = 0; i < __Sizeof(item_grp); i++)
    {
        lv_settings::item_t * item = &item_grp[i];
        if(i >= IIDX_Pitch && i <= IIDX_Yaw)
        {
            item_grp[i].state = *(int16_t*)item_grp[i].user_data.ptr;
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
    
    taskIMU_Update = lv_task_create(Task_IMU_Update, 100, LV_TASK_PRIO_MID, 0);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    Menu_AnimOpen(&menu, false);
    lv_task_del(taskIMU_Update);
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
void PageRegister_GyroscopeCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
