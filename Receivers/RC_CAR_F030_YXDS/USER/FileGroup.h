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
#include "MillisTaskManager.h"
#include "RC_Protocol.h"

//************* Objects *************//
extern NRF nrf;
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//
#define Buzz_Pin	PA0
#define LED_Pin		PB1
#define MotorL_Pin	PA10
#define MotorR_Pin	PA9
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

#define ConnectLost_TimeOut 200

#define MotorSpeed_MID 500

#define NRF_AddressConfig 45,23,78,19,61

#define LimitValue(x,min,max) (x=constrain(x,min,max))

extern uint8_t NRF_RxBuff[32];
extern Protocol_CarDS_t DS_CAR;
extern ConnectState_t ConnectState;

//*********** Functions ***********//
void Task_NRF_Receive();
void Task_CarCtrl();
void Task_MID_Setting();
void Task_BuzzRing_Block();
void Task_Test();

#endif
