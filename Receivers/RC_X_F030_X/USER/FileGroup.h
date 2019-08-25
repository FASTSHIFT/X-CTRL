#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

/** 
  *@Illustrate: A Cross-platform migration Project | Arduino for STM32F0xx 
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
#include "MusicPlay.h"

//************* Objects *************//
extern NRF nrf;
extern MillisTaskManager mtm;
extern Servo SteerServo;
extern Servo BrushlessMotor;
extern MusicPlay mp;

//*********** GPIO Define ***********//
#define Buzz_Pin	PA0
#define LED_Pin		PB1
#define Motor_Pin	PA10
#define Servo_Pin	PA9

#define NRF_IRQ_Pin		PA1
#define	NRF_MISO_Pin	PA6
#define NRF_MOSI_Pin	PA7
#define NRF_SCK_Pin		PA5
#define NRF_CSN_Pin		PA14
#define NRF_CE_Pin		PA13
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//*********** Value ***********//
#define ConnectLost_TimeOut 500
#define NRF_AddressConfig 45,23,78,19,61
#define LimitValue(x,min,max) (x=constrain(x,min,max))
#define NowCtrlObject X_COMMON
#define ServoAngle_MAX 148
#define ServoAngle_MIN 107
#define MotorSpeed_MAX 160
#define MotorSpeed_MIN 40

typedef Protocol_Common_t Protocol_CtrlType;
extern Protocol_CtrlType RC_X;
extern ConnectState_t ConnectState;
extern uint16_t ServoAngle, ServoAngle_MID;
extern uint16_t MotorSpeed, MotorSpeed_MID;

//*********** Functions ***********//
void Task_NRF_Receive();
void Task_Ctrl();
void Task_MID_Setting();
void Task_BuzzRing();
void Task_InfoLED();
void NRF_Init();

#endif
