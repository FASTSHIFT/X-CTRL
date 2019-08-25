#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************** Core *****************//
#include "stm32f10x.h"
#include "Arduino.h"
#include "math.h"
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"
#include "C_call_CPP.h"
#include "WMath.h"
#include "Tone.h"

//************* Arduino API **************//
#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"
#include "WString.h"
#include "Stream.h"

//************* User Libraries ************//
#include "extEEPROM.h"
#include "NRF.h"
#include "MPU6050.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "MillisTaskManager.h"
#include "Bluetooth_HC05.h"
#include "BottonEvent.h"
#include "EncoderEvent.h"
#include "RC_Protocol.h"

//**************Default Config**************//
#define _X_CTRL_VERSION "V3.0"
#define TaskManager_Timer TIM2
#define TaskManager_Freq 1000//us
#define SendFreq_Default 10//ms
#define CtrlMode_Default CtrlMode_Race
#define CtrlObject_Default X_COMMON
#define UART_Baud_Select_Default 10//115200
#define ADC_MaxValue 4095
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.2
#define MPU_ReadFreq_Default 10//ms
#define MPU_Max_Default 12000

#define ConnectLost_TimeOut 500

#define NRF_Address_Common 45,23,78,19,61
#define NRF_Address_CarDS  87,46,65,93,15
#define NRF_Address_CarSS  12,55,78,54,19

#define BlueTooth Serial
#define HMI Serial
#define LimitValue(x,min,max) (x=constrain(x,min,max))

//************* Object & Struct & Value**************//
extern NRF nrf;
extern MPU6050 mpu;
extern Adafruit_ST7735 tft;
extern MillisTaskManager mtm;
extern MillisTaskManager mtm_Display;
extern Bluetooth_HC05 hc05;

extern Protocol_CarDS_t CarDS;
extern Protocol_CarSS_t CarSS;
extern Protocol_Common_t CTRL;
extern ConnectState_t ConnectState;
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern Axis_t MPU_Data;
extern float BattUsage;
extern bool SilentMode;

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
#define clear() fillScreen(ST7735_BLACK)

//****************** Functions********************//
void Task_SensorUpdate();
void Task_TransferData();
void Task_LoadCtrlMode();
void Task_EncoderLED();
void Task_MPU6050Read();

void Init_X_CTRL();
void Init_Display();
void Init_NRF(uint8_t ctrlObject);
void Init_BottonEvent();
void Init_EncoderEvent();
void Init_GUI();

void BuzzRing(uint8_t chs);
void BottonEvent_Monitor();

void Encoder_Monitor();

void Menu_Up();
void Menu_Down();

#endif
