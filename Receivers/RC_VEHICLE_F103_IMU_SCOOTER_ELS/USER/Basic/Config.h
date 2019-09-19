#ifndef __CONFIG_H
#define __CONFIG_H

#define RCX_BOARD_VERSION 2

//*********** GPIO Define ***********//
#if (RCX_BOARD_VERSION == 1)

#define CH4_Pin     PA3//TIM2 CH4 ADC
#define CH3_Pin     PA2//TIM2 CH3 ADC
#define CH2_Pin     PA1//TIM2 CH2 ADC
#define CH1_Pin     PA0//TIM2 CH1 ADC
#define CH6_Pin     PB7//TIM4 CH2
#define CH5_Pin     PB6//TIM4 CH1

#define LED_Pin     PA15
#define SPD_Pin     PA11
#define MPU_INT_Pin PC0

#define NRF_IRQ_Pin     PB2
#define NRF_MISO_Pin    PA6
#define NRF_MOSI_Pin    PA7
#define NRF_SCK_Pin     PA5
#define NRF_CSN_Pin     PB1
#define NRF_CE_Pin      PB0
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#elif (RCX_BOARD_VERSION == 2)

#define USE_IMU
#define SDA_Pin PA11
#define SCL_Pin PA12

#define CH4_Pin     PA3//TIM2 CH4 ADC
#define CH3_Pin     PA2//TIM2 CH3 ADC
#define CH2_Pin     PA1//TIM2 CH2 ADC
#define CH1_Pin     PA0//TIM2 CH1 ADC
#define CH6_Pin     PB7//TIM4 CH2
#define CH5_Pin     PB6//TIM4 CH1

#define LED_Pin     PB4
#define SPD_Pin     PB5
#define MPU_INT_Pin PA8

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
#else
#error "Board verison is unsupport"
#endif

/*****************USER Define****************/
#define SPD_DET_Pin         CH6_Pin
#define Motor_Pin           CH4_Pin
#define BattDet_Pin         CH3_Pin
#define HandshakeEnable_Pin SPD_Pin

#define ConnectLost_TimeOut 500
#define SignalLostTimeout 5000

#define USE_WATCH_DOG
#define USE_POWER_DOWN

/*握手通信定时中断定时器*/
//#define TIM_Handshake TIM3

/*蜂鸣器定时器*/
#define TIM_BuzzTone    TIM1

/*时速计算定时器*/
#define TIM_CalcSpeed   TIM3

#endif
