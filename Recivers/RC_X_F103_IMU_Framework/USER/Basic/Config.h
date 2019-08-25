#ifndef __CONFIG_H
#define __CONFIG_H

//*********** GPIO Define ***********//
#define CH4_Pin     PA3//TIM2 CH4 ADC
#define CH3_Pin     PA2//TIM2 CH3 ADC
#define CH2_Pin     PA1//TIM2 CH2 ADC
#define CH1_Pin     PA0//TIM2 CH1 ADC
#define CH6_Pin     PB7//TIM4 CH2
#define CH5_Pin     PB6//TIM4 CH1

#define LED_Pin     PB4
#define SPD_DET_Pin PB5
#define MPU_INT_Pin     PA8
#define NRF_IRQ_Pin     PB1
#define NRF_MISO_Pin    PB0
#define NRF_MOSI_Pin    PA7
#define NRF_SCK_Pin     PA6
#define NRF_CSN_Pin     PA5
#define NRF_CE_Pin      PA4
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

/*****************USER Define****************/
#define Motor_L_Pin         CH4_Pin
#define Motor_R_Pin         CH3_Pin
//#define HandshakeEnable_Pin SPD_DET_Pin

#define ConnectLost_TimeOut 500
#define SignalLostTimeout (6*1000)

/*握手通信定时中断定时器*/
//#define TIM_Handshake TIM3

/*蜂鸣器定时器*/
#define TIM_BuzzTone    TIM1

#endif
