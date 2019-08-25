#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

/** 
  *@Illustrate: A Cross-platform migration Project | Arduino for STM32F0xx 
  *@Author: FASTSHIFT
  *@Website: https://github.com/FASTSHIFT/Arduino-For-Keil
  */

//************* Arduino Library **************//
#include "Arduino.h"
#include "Tone.h"

//************* STM32 Core Library **************//
#include "delay.h"
#include "gpio.h"
#include "pwm.h"
#include "timer.h"

//********** User Libraries *********//
#include "NRF.h"
#include "Servo.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"

//************* Objects *************//
extern NRF nrf;
extern Servo Rudder_L;
extern Servo Rudder_R;
extern Servo BrushlessMotor;
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//
#define Buzz_Pin	PA0
#define Motor_Pin	PA10
#define Rudder_L_Pin	PA9
#define Rudder_R_Pin	PB1
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

#define Rudder_L_MAX 180
#define Rudder_L_MIN 0
#define Rudder_R_MAX 180
#define Rudder_R_MIN 0

#define MotorSpeed_MAX 160
#define MotorSpeed_MIN 40

#define ConnectLost_TimeOut 500

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

extern float Rudder_L_Angle, Rudder_L_MID;
extern float Rudder_R_Angle, Rudder_R_MID;
extern uint16_t MotorSpeed, MotorSpeed_MID;

extern bool PassBack_State;

//*********** Functions ***********//
void Task_NRF_Receive();
void Task_Ctrl();
void Task_MID_Setting();
void Task_BuzzRing();
void NRF_Init();

#endif
