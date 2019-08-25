#include "FileGroup.h"

MillisTaskManager mtm(5);

void setup()
{
    Init_MotorServo();
    PWM_Init(FrontLED_Pin, 100, 1000);
    analogWrite(FrontLED_Pin, 0);
    Init_NRF();
    
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, HIGH);
    delay(80);
    digitalWrite(LED_Pin, LOW);
    delay(80);
    digitalWrite(LED_Pin, HIGH);
    delay(80);
    digitalWrite(LED_Pin, LOW);
    
    
    while(!HandshakeToMaster())
        __IntervalExecute(togglePin(LED_Pin), 2000);
    
    mtm.TaskRegister(0, Task_NRF_Handler, 10);
    mtm.TaskRegister(1, Task_Ctrl, 10);
    mtm.TaskRegister(2, Task_InfoLED, 100);
    mtm.TaskRegister(3, Task_MID_Setting, 100);
}

void loop()
{
    mtm.Running(millis());
}

void InternalClocks_Init()
{
    RCC_HSICmd(ENABLE);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    while(RCC_GetSYSCLKSource());
    RCC_PLLCmd(DISABLE);
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
    RCC_PLLCmd(ENABLE);
    while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_SYSCLK_Div1);
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    InternalClocks_Init();
    GPIO_JTAG_Disable();
//  SysClock_Init(F_CPU_128MHz);
    Delay_Init();
    ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
