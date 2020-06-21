#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

#define _X_CTRL_VERSION "v5.0"

//*************** STM32 Core *****************//
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
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

//************* Object & Struct & Value**************//
/*Object*/
extern Bluetooth_HC05 hc05;
extern ButtonEvent btUP;
extern ButtonEvent btDOWN;
extern ButtonEvent btOK;
extern ButtonEvent btBACK;
extern ButtonEvent btL1;
extern ButtonEvent btL2;
extern ButtonEvent btR1;
extern ButtonEvent btR2;
extern ButtonEvent btEcd;
extern EncoderEvent ecd;

/*Struct*/
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern Axis_t MPU_Data;

/*Value*/
extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool State_BuzzSound;
extern bool State_Bluetooth;
extern bool State_MPU;
extern bool State_PowerSaving;
extern bool State_DisplayCPU_Usage;
extern bool State_NoOperationMonitor;
extern float BattUsage;
extern float BattVoltage;
extern float CPU_Usage;
extern uint8_t Bluetooth_ConnectObject;

//****************** Functions********************//
void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void ButtonEventMonitor();

void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Init_X_CTRL();
bool Init_Value();
void Init_Joysticks();
void Init_BottonEvent();
void Init_Display();
void Init_EncoderEvent();
void Init_GUI();
void Init_HMI();
bool Init_NRF();

void SysClock_Config();

void Thread_HMI();
void Thread_GUI();
void Task_SensorUpdate();
void Task_TransferData();
void Task_MPU6050Read();
void Task_MusicPlayerRunning();
void Task_XFS_ListCheck();

void XFS_Clear();
void XFS_Speak(String str);
int XFS_Speak(const char *__restrict __format, ...);

#endif
