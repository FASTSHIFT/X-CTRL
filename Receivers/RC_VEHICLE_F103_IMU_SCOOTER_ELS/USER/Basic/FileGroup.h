#ifndef __FILEGROUP_H
#define __FILEGROUP_H

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

//********** User Libraries *********//
#include "NRF.h"
#include "Servo.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"
#include "Config.h"

//************* Objects *************//
extern NRF nrf;
extern MillisTaskManager mtm;

//*********** Value ***********//

enum TaskPriority{
    TP_NRF_Handler,
    TP_SpeedGenerator,
    TP_Ctrl,
    TP_InfoReport,
    TP_InfoLED,
    TP_FeedWatchDog,
    TP_MAX
};

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

extern bool IsLowPowerEnable;

//*********** Functions ***********//

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))

bool EEPROM_Register(void *pdata, uint16_t size);
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
void EnterSystemPowerMode();

void Init_NRF();
void Init_Motor();
void Init_Value();
void Init_MPU6050();
void Init_SpeedDet();
void Init_BaseClocks(bool internal);
void Init_IWDG(uint8_t prescaler, uint16_t reload);

bool HandshakeToMaster(uint8_t setState = 0xff);

void Task_NRF_Handler();
void Task_Ctrl();
void Task_InfoLED();
void Task_MPU6050Read();
void Task_InfoReport();
void Task_SpeedGenerator();

#endif
