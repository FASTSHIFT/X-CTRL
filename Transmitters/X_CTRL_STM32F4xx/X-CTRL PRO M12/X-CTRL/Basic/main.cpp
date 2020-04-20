#include "FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "MillisTaskManager/MillisTaskManager.h"

static MillisTaskManager mtmMain(8, true);
float CPU_Usage;

static void CPU_UsageUpdate()
{
    CPU_Usage = mtmMain.GetCPU_Usage();
}

static void Timer_Callback()
{
    mtmMain.Running(millis());
}

static void setup()
{
    X_CTRL_Init();
    
    mtmMain.TaskRegister(0, Com_Update, 10);
    mtmMain.TaskRegister(1, Button_Update, 10);
    mtmMain.TaskRegister(2, Joystick_Update, 10);
    mtmMain.TaskRegister(3, IMU_Update, 20);
    mtmMain.TaskRegister(4, Buzz_Update, 20);
    mtmMain.TaskRegister(5, Motor_Update, 20);
    mtmMain.TaskRegister(7, CPU_UsageUpdate, 1000);
    
    Timer_SetInterrupt(TIM_MTM_MAIN, 1000, Timer_Callback);
    TIM_Cmd(TIM_MTM_MAIN, ENABLE);
}

static void loop()
{
    Display_Update();
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
    Delay_Init();
    setup();
    for(;;)loop();
}
