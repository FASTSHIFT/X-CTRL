/** @Author: _VIFEXTech
  * @Describe:  
  * @Finished:
  */
  
#include "FileGroup.h"

static MillisTaskManager ControlTask(3, true);
static void Thread_Control();

static void SystemSetup()
{
    Init_TG_Ctrl();

    ControlTask.TaskRegister(0, Task_TransferData, 10);
    ControlTask.TaskRegister(1, Task_SensorUpdate, 10);
    ControlTask.TaskRegister(2, Task_MusicPlayerRunning, 20);    
    Timer_Init(ControlThread_TIM, 1000, Thread_Control, 1, 0);    
    TIM_Cmd(ControlThread_TIM, ENABLE);

    Init_GUI();
}

static void Thread_Main()
{
    Thread_GUI();
}

static void Thread_Control()
{
    ControlTask.Running(millis());
}


/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    GPIO_JTAG_Disable();
    Delay_Init();
//    ADCx_Init(ADC1);
    ADCx_DMA_Init();
    SystemSetup();
    for(;;)Thread_Main();
}
