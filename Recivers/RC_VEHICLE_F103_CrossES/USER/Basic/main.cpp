#include "FileGroup.h"

MillisTaskManager mtm(TP_MAX, true);
bool IWDG_Config(uint8_t prescaler, uint16_t reload);

void setup()
{
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, HIGH);
    Init_Motor();

    toneSetTimer(TIM3);
    pinMode(Buzz_Pin, OUTPUT);
    toneBlock(Buzz_Pin, 523, 80);
    digitalWrite(LED_Pin, LOW);
    toneBlock(Buzz_Pin, 880, 80);
    digitalWrite(LED_Pin, HIGH);
    toneBlock(Buzz_Pin, 659, 80);
    digitalWrite(LED_Pin, LOW);

    pinMode(BattDet_Pin, INPUT_ANALOG);
    pinMode(HandshakeEnable_Pin, INPUT_PULLUP);

    NRF_Init();
    
    if(!digitalRead(HandshakeEnable_Pin))//对频键按下时进入握手状态
    {
        while(!HandshakeToMaster())
            __IntervalExecute(togglePin(LED_Pin), 2000);
    }
    
    pinMode(NRF_IRQ_Pin, INPUT_PULLUP);
    EXTIx_Init(NRF_IRQ_Pin, NRF_IRQHandler, FALLING, 1, 1);
    
    IWDG_Config(IWDG_Prescaler_64, 625);

    mtm.TaskRegister(TP_NRF_Handler, Task_NRF_Handler, 10);
    mtm.TaskRegister(TP_SpeedGenerator, Task_SpeedGenerator, 10);
    mtm.TaskRegister(TP_Ctrl, Task_Ctrl, 10);
    mtm.TaskRegister(TP_BuzzRing, Task_BuzzRing, 20, OFF);
    mtm.TaskRegister(TP_InfoReport, Task_InfoReport, 200);
    mtm.TaskRegister(TP_InfoLED, Task_InfoLED, 100);
    mtm.TaskRegister(TP_FeedWatchDog, IWDG_ReloadCounter, 500);
}

void loop()
{
    mtm.Running(millis());
}

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
    while(RCC_GetSYSCLKSource() != 0x08);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_SYSCLK_Div1);
}

bool IWDG_Config(uint8_t prescaler, uint16_t reload)
{
    /*Tout=((4*2^prer)*rlr)/32 (ms)*/
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
	IWDG_SetPrescaler(prescaler); //设置IWDG分频系数
	IWDG_SetReload(reload);   //设置IWDG装载值
	IWDG_ReloadCounter(); //reload	
	IWDG_Enable();       //使能看门狗
    return true;
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    BaseClocks_Init();
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
