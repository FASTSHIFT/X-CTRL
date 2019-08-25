#include "FileGroup.h"

MillisTaskManager mtm(8);

void setup()
{
    PWM_Init(Motor1_Pin, 1000, 8000);
    PWM_Init(Motor2_Pin, 1000, 8000);
    analogWrite(Motor1_Pin, MotorSpeed_MID);
    analogWrite(Motor2_Pin, MotorSpeed_MID);
    ServoDir.attach(ServoDir_Pin);
    ServoDir.write(Servo_MID);

    toneSetTimer(TIM14);
    pinMode(Buzz_Pin, OUTPUT);
    toneBlock(Buzz_Pin, 523, 80);
    toneBlock(Buzz_Pin, 880, 80);
    toneBlock(Buzz_Pin, 659, 80);

    nrf.SetAddress(NRF_AddressConfig);
    NRF_Init();

    mtm.TaskRegister(0, Task_NRF_Receive, 10);
    mtm.TaskRegister(1, Task_Ctrl, 10);
    mtm.TaskRegister(2, Task_BuzzRing, 10);
    mtm.TaskRegister(3, Task_MID_Setting, 100);
}

void loop()
{
    mtm.Running(millis());
}

//*****************MAIN******************//

void BaseClocks_Init()
{
    RCC_HSICmd(ENABLE);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    while(RCC_GetSYSCLKSource());
    RCC_PLLCmd(DISABLE);
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
    RCC_PLLCmd(ENABLE);
    while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLKConfig(RCC_SYSCLK_Div1);
}

int main(void)
{
    BaseClocks_Init();
    Delay_Init();
//  ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
