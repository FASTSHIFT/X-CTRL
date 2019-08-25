#include "FileGroup.h"

MillisTaskManager mtm(6);
void IWDG_Config();

void setup()
{
    BrushlessMotor.attach(Motor_Pin);
    BrushlessMotor.write(MotorSpeed_MID);

    toneSetTimer(TIM14);
    pinMode(Buzz_Pin, OUTPUT);
    toneBlock(Buzz_Pin, 523, 80);
    toneBlock(Buzz_Pin, 880, 80);
    toneBlock(Buzz_Pin, 659, 80);
    
    pinMode(BattDet_Pin, INPUT_ANALOG);

    NRF_Init();
    while(!HandshakeToMaster());
    //IWDG_Config();
    mtm.TaskRegister(0, Task_NRF_Handler, 10);
    mtm.TaskRegister(1, Task_Ctrl, 20);
    mtm.TaskRegister(2, Task_BuzzRing, 20);
    mtm.TaskRegister(3, Task_SpeedGenerator, 10);
    mtm.TaskRegister(4, Task_InfoReport, 200);
    mtm.TaskRegister(5, IWDG_ReloadCounter, 400);
}

void loop()
{
    mtm.Running(millis());
}

void SysClock_Init()
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

bool IWDG_Config(float Time)
{
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
        RCC_ClearFlag();
    
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetReload(40000/32/2);// RL<4095, RT=0.5s 
    IWDG_ReloadCounter();
    IWDG_Enable();
    
    return true;
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    SysClock_Init();
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
