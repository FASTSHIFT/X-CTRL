#include "FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "MillisTaskManager/MillisTaskManager.h"

static MillisTaskManager mtmMain(8, true);

static void mtmMain_TimerCallback()
{
    mtmMain.Running(millis());
}

static void setup()
{
    X_CTRL_Init();
    
    mtmMain.TaskRegister(0, Com_Update, 10);
    mtmMain.TaskRegister(1, Joystick_Update, 10);
    mtmMain.TaskRegister(2, Button_Update, 20);
    mtmMain.TaskRegister(3, IMU_Update, 20);
    mtmMain.TaskRegister(4, Audio_Update, 20);
    mtmMain.TaskRegister(5, MotorLRA_Update, 20);
    
    Timer_SetInterruptBase(TIM_MTM_MAIN, 0xFF, 0xFF, mtmMain_TimerCallback, 2, 1);
    Timer_SetInterruptTimeUpdate(TIM_MTM_MAIN, 2000);
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
