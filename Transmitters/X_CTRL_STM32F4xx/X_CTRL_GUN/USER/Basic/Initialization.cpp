#include "FileGroup.h"
#include "ComPrivate.h"
#include "cm_backtrace.h"

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))//注册储存变量

#define IS_KEY_PRESSED(key,func) \
do{\
   if(!digitalRead(key))\
   {\
       delay(10);\
       if(!digitalRead(key))\
           func;\
   }\
}while(0)

/**
  * @brief  变量初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Init_Value()
{
    DEBUG_FUNC();
    /*注册需要掉电储保存的变量*/
    StorageDataReg(CTRL);                   //遥控器信息
    StorageDataReg(DR_ST_Value);            //转向限幅信息
    StorageDataReg(DR_TH_Value);            //油门限幅信息
    StorageDataReg(NRF_Cfg);                //NRF配置信息
    StorageDataReg(NRF_AddressConfig);      //NRF地址
    
    return EEPROM_Handle(EEPROM_Chs::ReadData);
}

/**
  * @brief  HC05(蓝牙)初始化
  * @param  无
  * @retval 无
  */
void Init_HC05()
{
    DEBUG_FUNC();
    hc05.Init(115200);          //波特率115200
    hc05.Power(CTRL.Bluetooth.Enable);//蓝牙使能
}

/**
  * @brief  遥控器初始化
  * @param  无
  * @retval 无
  */
void Init_X_CTRL()
{
    DEBUG_FUNC();
    cm_backtrace_init(_X_CTRL_NAME, _X_CTRL_VERSION , __DATE__);
    Init_Display();         //初始化显示器
    Init_Sensors();         //初始化传感器
    Init_HC05();            //初始化蓝牙
    Init_XBox360Sim();      //初始化XBox360模拟
    Init_Value();           //初始化变量

    IS_KEY_PRESSED(KEY_DOWN_Pin, CTRL.State.Sound = OFF);//按下按钮静音启动

    IS_KEY_PRESSED(KEY_SEL_Pin, CTRL.State.LuaScript = ON);

    /*判断是否成功初始化通信，并播放对应的提示音*/
    Com_Init() 
    ? BuzzMusic(MC_Type::MC_StartUp) 
    : BuzzMusic(MC_Type::MC_Error);
}
