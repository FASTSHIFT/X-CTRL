#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************** STM32 Core *****************//
#include "math.h"
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"
#include "WString.h"
#include "Stream.h"
#include "WMath.h"
#include "Tone.h"

//************* User Libraries ************//
#include "Bluetooth_HC05.h"
#include "ButtonEvent.h"
#include "EncoderEvent.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"

//**************Default Config**************//
#define _X_CTRL_VERSION "v4.5"
#define ADC_MaxValue 4095
#define TaskManager_Timer TIM2
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15
#define NRF_AddressNum 5
#define btLEFT btBACK
#define btRIGHT btOK

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

typedef struct {
    uint8_t Address;
    uint8_t Speed;
    uint8_t Freq;
} NRF_Config_TypeDef;

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
extern ConnectState_t ConnectState;
extern Protocol_CarDS_t CarDS;
extern Protocol_CarSS_t CarSS;
extern Protocol_Common_t CTRL;
extern Protocol_Passback_8xChannel_t CarSS_Passback;
extern Protocol_Passback_CommonDisplay_t Common_Passback;
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern Axis_t MPU_Data;
extern NRF_Config_TypeDef NRF_Cfg;

/*Value*/
extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool State_BuzzSound;
extern bool State_Bluetooth;
extern bool State_MPU;
extern bool State_PowerSaving;
extern float BattUsage;
extern float BattVoltage;
extern uint8_t NRF_AddressConfig[NRF_AddressNum * 5];
extern int16_t NRF_SignalStrength;
extern uint8_t Bluetooth_ConnectObject;

//*************GPIO**************//
#define DEFAULT_PIN PC0

#define JSL_X_Pin A7
#define JSL_Y_Pin A6
#define JSR_X_Pin A3
#define JSR_Y_Pin A2
#define ADL_Pin   A1
#define ADR_Pin   A0
#define BattSensor_Pin DEFAULT_PIN

#define EncoderLED_Pin DEFAULT_PIN
#define EncoderKey_Pin DEFAULT_PIN
#define EncoderA_Pin DEFAULT_PIN
#define EncoderB_Pin DEFAULT_PIN

#define Buzz_Pin D3
#define SPDT_Switch_Pin DEFAULT_PIN

#define Button_OK_Pin D10
#define Button_UP_Pin D8
#define Button_BACK_Pin D7
#define Button_DOWN_Pin D9
#define Button_R_Pin D6
#define Button_L_Pin D5
#define Button_JSL_Pin D12
#define Button_JSR_Pin D11

#define NRF_MOSI_Pin D11
#define NRF_MISO_Pin D12
#define NRF_SCK_Pin  D13
#define NRF_CE_Pin   D4
#define NRF_CSN_Pin  D2
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//****************** Functions********************//

void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void ButtonEventMonitor();

void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Thread_HMI();
void Thread_GUI();

void Init_X_CTRL();
bool Init_Value();
void Init_Display();
bool Init_NRF();
void Init_BottonEvent();
void Init_EncoderEvent();
void Init_GUI();
void Init_HMI();
void Init_XFS();
void Init_MPU6050();

void SysClock_Config();

void Task_SensorUpdate();
void Task_TransferData();
void Task_EncoderLED();
void Task_MPU6050Read();
void Task_XFS_ListCheck();
void Task_SignalMonitor();
void Task_ScreenDisplay();
void Task_MusicPlayerRunning();

void XFS_Speak(String str);
int XFS_Speak(const char *__restrict __format, ...);

#endif
