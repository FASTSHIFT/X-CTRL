#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

#define _X_CTRL_VERSION "v5.4"

//*************** STM32 Core *****************//
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "WMath.h"
#include "Tone.h"

//************* User Libraries ************//
#include "Bluetooth_HC05.h"
#include "ButtonEvent.h"
#include "EncoderEvent.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"
#include "Config.h"

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

namespace BC_Type {
typedef enum {
    BC_HMI,
    BC_XFS,
    BC_PHE,
    BC_END
} BluetoothConnect_Type;
}
extern String StrBtc[BC_Type::BC_END];

//************* Object & Struct & Value**************//
/*Object*/
extern Bluetooth_HC05 hc05;

extern ButtonEvent btUP;
extern ButtonEvent btDOWN;
extern ButtonEvent btOK;
extern ButtonEvent btBACK;

extern ButtonEvent btEcd;
extern EncoderEvent ecd;

/*Struct*/
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern Axis_t MPU_Data;

/*Value*/
extern bool State_BuzzSound;
extern bool State_Bluetooth;
extern bool State_MPU;
extern bool State_LuaScript;
extern bool State_PowerSaving;
extern bool State_DisplayCPU_Usage;
extern float BattUsage;
extern float BattVoltage;
extern uint8_t Bluetooth_ConnectObject;
extern float CPU_Usage;

//****************** Functions********************//
void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void ButtonEventMonitor();

void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Init_X_CTRL();
bool Init_Value();
void Init_Buzz();
void Init_BottonEvent();
void Init_Display();
void Init_EncoderEvent();
void Init_GUI();
void Init_HMI();
void Init_MPU6050();
bool Init_NRF();
void Init_XFS();
void Init_Motor();
bool Init_SD();
void Init_BvPlayer();
void Init_LuaScript();
void Init_XBox360Sim();

bool Keyboard_Print(char* str);

void MotorVibrate(float strength, uint32_t time);

void SysClock_Config();

void Thread_HMI();
void Thread_GUI();
void Task_SensorUpdate();
void Task_TransferData();
void Task_MPU6050Read();
void Task_MusicPlayerRunning();
void Task_XFS_ListCheck();
void Task_MotorRunning();

void XFS_Clear();
void XFS_Speak(String str);
int XFS_Speak(const char *__restrict __format, ...);

#endif
