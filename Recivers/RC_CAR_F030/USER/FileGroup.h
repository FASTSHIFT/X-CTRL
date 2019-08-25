#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************C/C++ Library**************//

#include "stm32f0xx.h"
#include "Arduino.h"
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "usart.h"
#include "timer.h"
#include "math.h"
#include "WMath.h"
#include "Tone.h"

#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"
#include "WString.h"
#include "Stream.h"

#include "C_call_CPP.h"

//********** User Libraries *********//
#include "NRF.h"
#include "Servo.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "DigitalFilter.h"
#include "MPU6050.h"

//************* Objects *************//
extern NRF nrf;
extern Servo SteerServo;
extern Servo BrushlessMotor;
extern MillisTaskManager mtm;
extern MPU6050 mpu;

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
//#define HQ_734
#define HQ_727

#ifdef HQ_734
	#define ServoAngle_MAX 131
	#define ServoAngle_MIN 40
	#define MotorSpeed_MAX 120
	#define MotorSpeed_MIN 60
#endif

#ifdef HQ_727
	#define ServoAngle_MAX 153
	#define ServoAngle_MIN 110
	#define MotorSpeed_MAX 120
	#define MotorSpeed_MIN 40
#endif

#define ConnectLost_TimeOut 200

#define NRF_Address_Common 11,22,33,22,11
#define NRF_Address_CarDS  11,22,33,44,55
#define NRF_Address_CarSS  55,44,33,22,11
#define NRF_Detect() (nrf.Status()!=0x00&&nrf.Status()!=0xFF)

extern Protocol_CarSS_t RC_CAR;
extern ConnectState_t ConnectState;

extern uint16_t ServoAngle, ServoAngle_MID;
extern uint16_t MotorSpeed, MotorSpeed_MID;

//*********** Functions ***********//
void Task_NRF_Receive();
void Task_CarCtrl();
void Task_MID_Setting();
void Task_BuzzRing();
void Task_BuzzRing_Block();
void Task_Test();

#endif
