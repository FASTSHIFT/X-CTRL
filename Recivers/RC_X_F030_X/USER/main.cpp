#include "FileGroup.h"

MillisTaskManager mtm(10);

void setup()
{
    BrushlessMotor.attach(Motor_Pin);
	BrushlessMotor.write(MotorSpeed_MID);
    SteerServo.attach(Servo_Pin);
	SteerServo.write(ServoAngle_MID);
	
	toneSetTimer(TIM14);
    pinMode(Buzz_Pin, OUTPUT); 
    toneBlock(Buzz_Pin, 523, 80);
    toneBlock(Buzz_Pin, 880, 80);
    toneBlock(Buzz_Pin, 659, 80);
	
	PWM_Init(LED_Pin, 100, 1000);
	
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

void BaseClocks_Init()
{
	RCC_HSICmd(ENABLE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	while(RCC_GetSYSCLKSource());
	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_12);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLKConfig(RCC_SYSCLK_Div1);
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
//  ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
