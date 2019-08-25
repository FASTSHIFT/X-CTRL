#ifndef __STM32_NANO_H
#define __STM32_NANO_H

#define A0 PA0  //ADC(ADC1 CH0)  PWM(TIM2 CH1)
#define A1 PA1  //ADC(ADC1 CH1)  PWM(TIM2 CH2)
#define A2 PA2  //ADC(ADC1 CH2)  PWM(TIM2 CH3)  USART2-TX
#define A3 PA3  //ADC(ADC1 CH3)  PWM(TIM2 CH4)  USART2-RX
#define A4 PA4  //ADC(ADC1 CH4)  -------------
#define A5 PA5  //ADC(ADC1 CH5)  -------------
#define A6 PA6  //ADC(ADC1 CH6)  PWM(TIM3 CH1)
#define A7 PA7  //ADC(ADC1 CH7)  PWM(TIM3 CH2)

#define D0 PA9  //-------------  PWM(TIM1 CH2)  USART1-TX
#define D1 PA10 //-------------  PWM(TIM1 CH3)  USART1-RX
#define D2 PB2  //-------------  -------------
#define D3 PB0  //ADC(ADC1 CH8)  PWM(TIM3 CH3)
#define D4 PA15 //-------------  -------------
#define D5 PB1  //ADC(ADC1 CH9)  PWM(TIM3 CH4)
#define D6 PA8  //-------------  PWM(TIM1 CH1)
#define D7 PB4  //-------------  -------------
#define D8 PB5  //-------------  -------------
#define D9 PB6  //-------------  PWM(TIM4 CH1)
#define D10 PB7 //-------------  PWM(TIM4 CH2)
#define D11 PA11//-------------  PWM(TIM1 CH4)
#define D12 PA12//-------------  -------------
#define D13 PB3 //-------------  -------------

#define SDA_Pin PA4
#define SCL_Pin PA5

#endif
