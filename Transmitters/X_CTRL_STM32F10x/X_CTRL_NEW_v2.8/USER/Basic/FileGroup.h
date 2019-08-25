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
#include "Adafruit_ST7735.h"
#include "Bluetooth_HC05.h"
#include "ButtonEvent.h"
#include "DigitalFilter.h"
#include "EncoderEvent.h"
#include "MPU6050.h"
#include "MillisTaskManager.h"
#include "NRF.h"
#include "PageManager.h"
#include "RC_Protocol.h"


//**************Default Config**************//
#define _X_CTRL_VERSION "v2.8"
#define ADC_MaxValue 4095
#define TaskManager_Timer TIM2
#define TaskManager_Freq 1000//us
#define SendFreq_Default 10//ms
#define CtrlMode_Default CtrlMode_Race
#define CtrlObject_Default X_COMMON
#define UART_BaudSelect_Default 10//115200
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15
#define MPU_ReadFreq_Default 20//ms
#define MPU_Max_Default 60
#define ConnectLost_TimeOut 500
#define NRF_AddressNum 5
#define BlueTooth Serial
#define HMI Serial
#define btLEFT btBACK
#define btRIGHT btOK

#define LimitValue(x,min,max) (x=constrain(x,min,max))

typedef enum{
	ReadData,
	SaveData,
	ClearData
}EEPROM_Chs_t;

typedef struct{
    uint8_t Address;
    uint8_t Speed;
    uint8_t Freq;
}NRF_Config_TypeDef;

//************* Object & Struct & Value**************//
/*Object*/
extern NRF nrf;
extern MPU6050 mpu;
extern Adafruit_ST7735 screen;
extern MillisTaskManager mtm_StatusBar;
extern MillisTaskManager mtm_Main;
extern MillisTaskManager mtm_HMI;
extern Bluetooth_HC05 hc05;
extern PageManager pm;
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
extern float BattUsage;
extern float BattVoltage;
extern bool BuzzSound_State;
extern bool RF_State;
extern bool PassBack_State;
extern bool MPU_State;
extern bool HMI_State;
extern bool Handshake_State;
extern uint8_t NRF_AddressConfig[NRF_AddressNum * 5];
extern uint8_t NRF_SignalStrength;

//*************GPIO**************//
#define JSL_X_Pin PA0
#define JSL_Y_Pin PA1
#define JSR_X_Pin PA2
#define JSR_Y_Pin PA3
#define ADL_Pin PA4
#define ADR_Pin PA5
#define BattSensor_Pin PA6

#define EncoderLED_Pin PB1 //Use TIM3
#define EncoderKey_Pin PB9
#define EncoderA_Pin PB8
#define EncoderB_Pin PB7

#define Buzz_Pin PB12
#define SPDT_Switch_Pin PA15

#define HC165_OUT_Pin		PB4
#define HC165_CP_Pin		PB5
#define HC165_PL_Pin		PB6

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin PB13
#define NRF_CE_Pin PA12
#define NRF_CSN_Pin PA11
#define NRF_IRQ_Pin -1
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin PC13
#define BT_State_Pin PC14
#define BT_EN_Pin PC15

#define TFT_CS     -1
#define TFT_RST    -1
#define TFT_DC     PB0
#define TFT_MOSI	PB3
#define TFT_CLK		PA8

//****************** Functions********************//
void Task_SensorUpdate();
void Task_TransferData();
void Task_EncoderLED();
void Task_MPU6050Read();

void Init_X_CTRL();
void Init_Display();
void Init_NRF();
void Init_BottonEvent();
void Init_EncoderEvent();
void Init_GUI();
void Init_HMI();

void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void BuzzSignalMonitor();
void ButtonEventMonitor();
void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs_t chs);
void HMI_Running();

void PageRegister_MainMenu();
void PageRegister_CtrlInfo();
void PageRegister_Options();
void PageRegister_SetJoystick();
void PageRegister_SetBluetooth();
void PageRegister_SetGravity();
void PageRegister_About();
void PageRegister_Handshake();

#endif
