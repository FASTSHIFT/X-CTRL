/** @Author: _VIFEXTech
  * @Describe: <RC-Ctrl>
  * @Finished: 2018.11.4  - v1.0
  * @Upgrade:  2018.11.9  - v1.1 移植了所有X-CTRL代码
  * @Upgrade:  2018.11.25 - v1.2 同步 X-CTRL v1.8
  * @Upgrade:  2018.11.30 - v1.3 同步 X-CTRL v2.1
  */

#include "FileGroup.h"

MillisTaskManager mtm_Ctrl(4);

void TaskManager_Loop();

void setup()
{
    Init_RC_Ctrl();

    mtm_Ctrl.TaskRegister(0, Task_SensorUpdate, 5);
    mtm_Ctrl.TaskRegister(1, Task_TransferData, SendFreq_Default);
    mtm_Ctrl.TaskRegister(2, Task_MPU6050Read, MPU_ReadFreq_Default);
    mtm_Ctrl.TaskRegister(3, Task_OledDisplay, 30);
    Timer_Init(TaskManager_Timer, TaskManager_Freq, TaskManager_Loop, 1, 1);
    Init_GUI();
}

void loop()
{
    mtm_StatusBar.Running(millis());
    pm.Running();
    HMI_Running();
}

void TaskManager_Loop()
{
    mtm_Ctrl.Running(millis());
}

//*****************MAIN******************//
int main(void)
{
    GPIO_JTAG_Disable();
    Delay_Init();
//  ADCx_Init(ADC1);
    ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
