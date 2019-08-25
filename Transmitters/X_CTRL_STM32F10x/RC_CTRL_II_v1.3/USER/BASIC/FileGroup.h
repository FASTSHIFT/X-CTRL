#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

/** 
  *@Illustrate: A Cross-platform migration Project | Arduino for STM32F10x 
  *@Author: FASTSHIFT
  *@Website: https://github.com/FASTSHIFT/Arduino-For-Keil
  */

//************* Arduino Library **************//
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Stream.h"
#include "SPI.h"
#include "WString.h"
#include "Wire.h"
#include "Tone.h"
#include "WMath.h"

//************* STM32 Core Library **************//
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"

//************************** USER ******************************//
#define _RC_CTRL_VERSION "v1.3"
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
#define NRF_AddressNum 5
#define ConnectLost_TimeOut 500
#define HMI Serial
#define BlueTooth Serial
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

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "MPU6050.h"
#include "MillisTaskManager.h"
#include "NRF.h"
#include "ButtonEvent.h"
#include "RC_Protocol.h"
#include "PageManager.h"
#include "Bluetooth_HC05.h"
#include "DigitalFilter.h"
#include "EEPROM.h"

//******** GPIO Define **********//
//NRF
#define NRF_MOSI_Pin D11
#define NRF_MISO_Pin D12
#define NRF_SCK_Pin  D13
#define NRF_CE_Pin   D4
#define NRF_CSN_Pin  D2
/**NRF24L01
  *----------------+-------+
  *|               |       |
  *|        _______|_______|
  *|        |8:IRQ | 7:MISO|
  *|        |6:MOSI| 5:SCK |
  *|        |4:CSN | 3:CE  |
  *|        |2:VCC | 1:GND |
  *----------------+-------+
  */

//ADC
#define JSL_X_Pin A7
#define JSL_Y_Pin A6
#define JSR_X_Pin A3
#define JSR_Y_Pin A2
#define ADL_Pin   A1
#define ADR_Pin   A0

//Others
#define Button_OK_Pin D10
#define Button_UP_Pin D8
#define Button_BACK_Pin D7
#define Button_DOWN_Pin D9
#define Button_R_Pin D6
#define Button_L_Pin D5
#define Button_JSL_Pin D12
#define Button_JSR_Pin D11

#define Buzz_Pin D3

//************ TypeDef & Object & Value ************//
extern Adafruit_SSD1306 oled;
extern MPU6050 mpu;
extern NRF nrf;
extern PageManager pm;
extern MillisTaskManager mtm_StatusBar;
extern Bluetooth_HC05 hc05;

extern Protocol_CarDS_t CarDS;
extern Protocol_CarSS_t CarSS;
extern Protocol_Common_t CTRL;
extern Protocol_Passback_8xChannel_t CarSS_Passback;
extern Protocol_Passback_CommonDisplay_t Common_Passback;
extern ConnectState_t ConnectState;
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern Axis_t MPU_Data;
extern float BattUsage;
extern float BattVoltage;
extern bool BuzzSound_State;
extern bool RF_State;
extern bool PassBack_State;
extern bool MPU_State;
extern bool HMI_State;
extern uint8_t NRF_AddressConfig[NRF_AddressNum * 5];
extern uint8_t SignalStrength;
extern NRF_Config_TypeDef NRF_Cfg;

extern ButtonEvent btUP;
extern ButtonEvent btDOWN;
extern ButtonEvent btOK;
extern ButtonEvent btBACK;
extern ButtonEvent btL1;
extern ButtonEvent btR1;
extern ButtonEvent btL2;
extern ButtonEvent btR2;

//*************** Function ***************//
void Task_SensorUpdate();
void Task_TransferData();
void Task_MPU6050Read();
void Task_OledDisplay();

void Init_RC_Ctrl();
void Init_Module();
void Init_GPIO();
void Init_GUI();
void Init_Button();
void Init_NRF();
void Init_StatusBar();
void Init_HMI();
void Init_Buzz();

void ButtonEvent_Monitor();
void BuzzMusic(uint8_t music);
void BuzzSignalMonitor();
void BuzzTone(uint32_t freq, uint32_t time);

void PageRegister_MainMenu();
void PageRegister_CtrlInfo();
void PageRegister_Options();
void PageRegister_SetJoystick();
void PageRegister_SetBluetooth();
void PageRegister_SetGravity();
void PageRegister_About();

void HMI_Running();
bool EEPROM_Handle(EEPROM_Chs_t chs);

#endif
