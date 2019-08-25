//Designed By _VIFEXTech
//New Version X-CTRL

#include "FileGroup.h"

MillisTaskManager mtm(10);

void loop_TaskManager();

void setup()
{	
    Init_X_CTRL();

    mtm.TaskRegister(0, Task_SensorUpdate, 5);
    mtm.TaskRegister(1, Task_LoadCtrlMode, 5);
    mtm.TaskRegister(2, Task_TransferData, 10);
    mtm.TaskRegister(3, Task_EncoderLED, 30);
	mtm.TaskRegister(4, Task_MPU6050Read, 20);
    Timer_Init(TaskManager_Timer, TaskManager_Freq, loop_TaskManager, 1, 1);

    Init_GUI();
    TIM_Cmd(TaskManager_Timer, ENABLE);
}

void loop()
{
    mtm_Display.Running(millis());
}

void loop_TaskManager()
{
    mtm.Running(millis());
}


//*****************MAIN******************//
int main()
{
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
