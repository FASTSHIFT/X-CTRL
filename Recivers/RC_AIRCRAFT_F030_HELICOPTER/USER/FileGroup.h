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

//************* Objects *************//
extern NRF nrf;
extern Servo ServoDir;
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//
#define Buzz_Pin	PA0
#define Motor1_Pin	PA10
#define Motor2_Pin	PA9
#define ServoDir_Pin	PB1
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

#define MotorSpeed_MID 0
#define Servo_MAX 180
#define Servo_MIN 20

#define NRF_AddressConfig 45,23,78,19,61
#define LimitValue(x,min,max) (x=constrain(x,min,max))

typedef struct{
	float Channel_1;
	float Channel_2;
	float Channel_3;
	float Channel_4;
	float Channel_5;
	float Channel_6;
	float Channel_7;
	float Channel_8;
}Slave_DataPack_t;

extern uint8_t NRF_RxBuff[32];
extern Protocol_Common_t RC_Aircraft;
extern ConnectState_t ConnectState;
extern bool PassBack_State;
extern int Servo_MID;

//*********** Functions ***********//
void Task_NRF_Receive();
void Task_Ctrl();
void Task_MID_Setting();
void Task_BuzzRing();
void NRF_Init();

#endif
