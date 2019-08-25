#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//************* Arduino Library **************//
#include "Arduino.h"
#include "Tone.h"

//************* STM32 Core Library **************//
#include "delay.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"

//********** User Libraries *********//
#include "NRF.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"

//************* Objects *************//
extern NRF nrf;
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//

#define LED_Pin PA1
#define Fan_Pin PB1

#define	NRF_MISO_Pin	PA5
#define NRF_MOSI_Pin	PA3
#define NRF_SCK_Pin		PA6
#define NRF_CSN_Pin		PA4
#define NRF_CE_Pin		PA2
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//*********** Value ***********//
#define ConnectLost_TimeOut 500
#define NRF_AddressConfig /*75,75,75,75,75*/ 13,41,39,81,82
#define LimitValue(x,min,max) (x=constrain(x,min,max))
#define NowCtrlObject Wand_XSuperWand

typedef Protocol_XSuperWand Protocol_CtrlType;
extern Protocol_CtrlType RC_X;
extern ConnectState_t ConnectState;

//*********** Functions ***********//
void Task_NRF_Receive();
void Task_Ctrl();

void NRF_Init();

#endif
