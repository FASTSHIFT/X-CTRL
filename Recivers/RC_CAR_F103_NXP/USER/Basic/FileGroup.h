#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

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
#include "MillisTaskManager.h"
#include "CommonMacro.h"

//************* Objects *************//
extern MillisTaskManager mtm;

//*********** GPIO Define ***********//
#define CH4_Pin		PA3//TIM2 CH4
#define CH3_Pin		PA2//TIM2 CH3
#define CH2_Pin		PA1//TIM2 CH2
#define CH1_Pin		PA0//TIM2 CH1
#define CH6_Pin		PB7//TIM4 CH2
#define CH5_Pin		PB6//TIM4 CH1


#define LED_Pin		PA15
#define SPD_DET_Pin PA11

#define Motor_Pin   CH4_Pin
#define Servo_Pin   CH3_Pin

#define NRF_IRQ_Pin		PB2
#define	NRF_MISO_Pin	PA6
#define NRF_MOSI_Pin	PA7
#define NRF_SCK_Pin		PA5
#define NRF_CSN_Pin		PB1
#define NRF_CE_Pin		PB0
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//*********** Value ***********//
enum TaskPriority{
    TP_NRF_Handler,
    TP_Ctrl,
    TP_InfoLED,
    TP_MID_Setting
};

//*********** Functions ***********//
bool HandshakeToMaster();

void Init_NRF();
void Init_MotorServo();

void Task_NRF_Handler();
void Task_Ctrl();
void Task_InfoLED();
void Task_MID_Setting();

#endif
