#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin  PB13
#define NRF_CE_Pin   PH3
#define NRF_CSN_Pin  PB12
#define NRF_IRQ_Pin  PA8

#define CH1_Pin      PB8 //TIM4 CH3
#define CH2_Pin      PB7 //TIM4 CH4
#define CH3_Pin      PB8 //TIM2 CH3
#define CH4_Pin      PB9 //TIM2 CH4
#define CH5_Pin      PA0 //TIM2 CH1
#define CH6_Pin      PA1 //TIM2 CH2
#define CH7_Pin      PA2 //TIM2 CH3
#define CH8_Pin      PA3 //TIM2 CH4

#define LED_Pin      PC15
#define BAT_DET_Pin  CH1_Pin

#define Motor_Pin    CH5_Pin
#define Servo_Pin    CH6_Pin

#endif
