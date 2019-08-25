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
#include "Tone.h"
#include "WString.h"
#include "Wire.h"
#include "WMath.h"

//************* STM32 Core Library **************//
#include "adc.h"
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "pwm.h"
#include "timer.h"

//************* User Library **************//
#include "CommonMacro.h"
#include "RC_Protocol.h"
#include "MillisTaskManager.h"

//************* Defines **************//
#define TaskManager_Timer TIM2
#define ADC_MaxValue 4095
#define Joystick_X_DMA_CH 0
#define Joystick_Y_DMA_CH 1
#define BattSensor_DMA_CH 2
/* GPIO */
#define Buzz_Pin D13
#define BattSensor_Pin A7
#define Button_UP_Pin   D6
#define Button_DOWN_Pin D7
#define Button_LEFT_Pin D5
#define Button_RIGHT_Pin D4
#define Button_POWER_Pin D3
#define Button_SHIFT_Pin A2

#define Joystick_X_Pin A0
#define Joystick_Y_Pin A1

#define HC165_OUT_Pin   A6
#define HC165_CP_Pin    D2
#define HC165_PL_Pin    A5

#define NRF_MOSI_Pin D9
#define NRF_MISO_Pin D8
#define NRF_SCK_Pin  D10
#define NRF_CE_Pin   D12
#define NRF_CSN_Pin  D11
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//************* Enum & TypeDef **************//

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

//*** Extern Object & Value & Struct ***//
extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool State_BuzzSound;
extern float BattVoltage, BattUsage;

extern Joystick_t JS_L,JS_R;

//************* Functions **************//
void BuzzTone(uint32_t freq, uint32_t time);
void BuzzMusic(uint8_t music);
void ButtonEventMonitor();

bool EEPROM_Register(void *pdata, uint16_t size);
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);

void Init_JW_CTRL();
bool Init_NRF();
void Init_GUI();
void Init_Screen();
void Init_BottonEvent();

void Thread_GUI();
void Task_SensorUpdate();
void Task_ScreenUpdate();
void Task_SignalMonitor();
void Task_TransferData();

#endif
