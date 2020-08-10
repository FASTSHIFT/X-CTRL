#include "FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "MillisTaskManager/MillisTaskManager.h"

static MillisTaskManager mtmMain(true);

static void mtmMain_TimerCallback()
{
    mtmMain.Running(millis());
}

static void setup()
{
    X_CTRL_Init();
    
    mtmMain.Register(Com_Update, 10);
    mtmMain.Register(Joystick_Update, 10);
    mtmMain.Register(Button_Update, 20);
    mtmMain.Register(IMU_Update, 20);
    mtmMain.Register(Audio_Update, 20);
    mtmMain.Register(MotorLRA_Update, 20);
    
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
