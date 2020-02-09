#ifndef __FILEGROUP_H
#define __FILEGROUP_H

#define _X_CTRL_VERSION "v9.4"
#define _X_CTRL_NAME    "X-CTRL_GUN"

//*************** STM32 Core *****************//
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"
#include "rng.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "WMath.h"
#include "Tone.h"

//************* User Libraries ************//
#include "Bluetooth_HC05.h"
#include "ButtonEvent.h"
#include "EncoderEvent.h"
#include "MillisTaskManager.h"
#include "CommonMacro.h"
#include "RCX/RCX_Type.h"
#include "SysConfig.h"
#include "XC_Type.h"

//************* Namespeac & Object & Struct & Value**************//

/*主调度器优先级分配表*/
enum TaskPriority
{
    TP_TransferData,
    TP_SensorUpdate,
    TP_IMU_Process,
    TP_CPUInfoUpdate,
    TP_MotorRunning,
    TP_MusicPlayerRunning,
    TP_XFS_ListCheck,
    TP_MAX
};
extern MillisTaskManager MainTask;

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

namespace BC_Type {
typedef enum {
    BC_XFS,
    BC_PHE,
    BC_END
} BluetoothConnect_Type;
}

namespace MC_Type {
typedef enum {
    MC_StartUp,
    MC_Error,
    MC_Connect,
    MC_Disconnect,
    MC_UnstableConnect,
    MC_BattChargeStart,
    MC_BattChanargeEnd,
    MC_DeviceInsert,
    MC_DevicePullout,
    MC_NoOperationWarning,
    MC_MAX
}MusicCode_ID_Type;
}

/*Object*/
extern Bluetooth_HC05 hc05;

extern ButtonEvent btUP;
extern ButtonEvent btDOWN;
extern ButtonEvent btOK;
extern ButtonEvent btBACK;
extern ButtonEvent btEcd;
extern EncoderEvent ecd;

extern CTRL_TypeDef CTRL;

//****************** Functions********************//
void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void ButtonEventMonitor();

void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Init_X_CTRL();
bool Init_Value();
void Init_BottonEvent();
void Init_Display();
void Init_EncoderEvent();
void Init_GUI(uint8_t step);
void Init_XFS();
bool Init_SD();
void Init_Sensors();
bool Init_BvPlayer();
void Init_LuaScript();
void Init_XBox360Sim();

bool Keyboard_Print(char* str);

void MotorVibrate(float strength, uint32_t time);

double NonlinearMap(double value, double in_min, double in_max, double out_min, double out_max, double startK, double endK);

bool SD_GetReady();

bool BvPlayer_GetReady();

void Thread_GUI();
void Thread_SD_Monitor();
void Task_SensorUpdate();
void Task_TransferData();
void Task_IMU_Process();
void Task_MusicPlayerRunning();
void Task_XFS_ListCheck();
void Task_MotorRunning();
void Task_CPUInfoUpdate();

void XFS_Clear();
void XFS_Speak(String str);
int XFS_Speak(const char *__restrict __format, ...);

#endif
