#include "FileGroup.h"

MillisTaskManager mtm(5);

void setup()
{
    Init_MotorServo();
    Init_NRF();
    //Init_IMU();
    while(!HandshakeToMaster());

    mtm.TaskRegister(0, Task_NRF_Handler, 10);
    mtm.TaskRegister(1, Task_Ctrl, 10);
    //mtm.TaskRegister(2, Task_IMUCalc, 20);
//    pinMode(IMU_IRQ_Pin, INPUT_PULLUP);
//    attachInterrupt(IMU_IRQ_Pin, Task_IMUCalc, RISING);
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
