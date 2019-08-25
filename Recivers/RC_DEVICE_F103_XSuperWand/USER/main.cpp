#include "FileGroup.h"

MillisTaskManager mtm(10);

void setup()
{
    pinMode(LED_Pin, PWM);
    pinMode(Fan_Pin, PWM);
    NRF_Init();

    mtm.TaskRegister(0, Task_NRF_Receive, 10);
    mtm.TaskRegister(1, Task_Ctrl, 10);
}

void loop()
{
    mtm.Running(millis());
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
	GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
