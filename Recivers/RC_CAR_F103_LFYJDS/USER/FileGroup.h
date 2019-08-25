#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************C/C++ Library**************//

#include "stm32f10x.h"
#include "Arduino.h"
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
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
#define Buzz_Pin PB9
#define InfoLED_Pin PC13
#define BattVoltageSample_Pin PA0
#define LED_Pin		PA3

#define MotorL_1_Pin PA2
#define MotorL_2_Pin PA4
#define MotorL_Pin	PA11

#define MotorR_1_Pin PA5
#define MotorR_2_Pin PA6
#define MotorR_Pin	PB1

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB13
#define NRF_SCK_Pin PB12
#define NRF_CE_Pin PA7
#define NRF_CSN_Pin PB10
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND


//*********** Value ***********//

#define ServoAngle_MAX 153
#define ServoAngle_MIN 110
#define MotorSpeed_MAX 120
#define MotorSpeed_MIN 40

#define ConnectLost_TimeOut 200

#define MotorSpeed_MID 0

#define NRF_AddressConfig 45,23,78,19,61
#define NRF_Detect() (nrf.Status()!=0x00&&nrf.Status()!=0xFF)

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
