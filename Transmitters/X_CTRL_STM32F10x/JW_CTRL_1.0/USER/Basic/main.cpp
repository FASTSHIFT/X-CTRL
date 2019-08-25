#include "FileGroup.h"
static MillisTaskManager CtrlThread(5);

void Thread_Control();

void setup()
{
    Init_JW_CTRL();
    CtrlThread.TaskRegister(0, Task_SensorUpdate, 10);
    CtrlThread.TaskRegister(1, Task_TransferData, 10);
    Timer_Init(TaskManager_Timer, 1000, Thread_Control, 1, 1);
    TIM_Cmd(TaskManager_Timer, ENABLE);
    Timer_Init(TIM3, 25*1000, Task_ScreenUpdate, 2, 0);
    TIM_Cmd(TIM3, ENABLE);
    Init_GUI();
}

void loop()
{
    Thread_GUI();
}

void Thread_Control()
{
    CtrlThread.Running(millis());
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	GPIO_JTAG_Disable();
//  SysClock_Init(F_CPU_128MHz);
    Delay_Init();
//  ADCx_Init(ADC1);
	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
