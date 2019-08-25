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


/*Value*/
extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool State_BuzzSound;
extern float BattUsage;
extern float BattVoltage;
extern uint8_t Bluetooth_ConnectObject;

//*************GPIO**************//
#define DEFAULT_PIN PC0

#define JSL_X_Pin PA3
#define JSL_Y_Pin PA2
#define JSR_X_Pin PA0
#define JSR_Y_Pin PA1
#define ADL_Pin   DEFAULT_PIN
#define ADR_Pin   DEFAULT_PIN
#define BattSensor_Pin PA4

#define JSL_X_ADC() analogRead_DMA(JSL_X_Pin)
#define JSL_Y_ADC() (ADC_MaxValue-analogRead_DMA(JSL_Y_Pin))
#define JSR_X_ADC() analogRead_DMA(JSR_X_Pin)
#define JSR_Y_ADC() analogRead_DMA(JSR_Y_Pin)
#define ADL_ADC()   ADC_MaxValue
#define ADR_ADC()   ADC_MaxValue
#define BattSensor_ADC()    (analogRead_DMA(4))

#define EncoderLED_Pin DEFAULT_PIN
#define EncoderKey_Pin PB15
#define EncoderA_Pin PB13
#define EncoderB_Pin PB14

#define Buzz_Pin PB11
#define SPDT_Switch_Pin PB12

#define Button_UP_Pin DEFAULT_PIN
#define Button_DOWN_Pin DEFAULT_PIN
#define Button_OK_Pin DEFAULT_PIN
#define Button_BACK_Pin DEFAULT_PIN

//#define Button_R_Pin A7
//#define Button_L_Pin A6
//#define Button_JSL_Pin DEFAULT_PIN
//#define Button_JSR_Pin DEFAULT_PIN

#define NRF_MOSI_Pin PB5
#define NRF_MISO_Pin PB7
#define NRF_SCK_Pin  PB8
#define NRF_CE_Pin   PB9
#define NRF_CSN_Pin  PB4
#define NRF_IRQ_Pin  PB6
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

void Thread_GUI();

void Init_X_CTRL();
bool Init_Value();
void Init_Display();
bool Init_NRF();
void Init_BottonEvent();
void Init_EncoderEvent();
void Init_GUI();

void SysClock_Config();

void Task_SensorUpdate();
void Task_TransferData();
void Task_EncoderLED();
void Task_SignalMonitor();
void Task_ScreenDisplay();
void Task_MusicPlayerRunning();

#define XFS_Speak(...)

#endif
