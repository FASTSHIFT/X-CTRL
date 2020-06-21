#include "FileGroup.h"
#include "TasksManage.h"

uint32_t RunTimeCounter = 0;

static void TimerForRuntime_Callback()
{
    RunTimeCounter++;
}

void AppConfigureTimerForRuntimeStats()
{
    Timer_SetInterruptBase(TIM_OS_RUNTIME, 0xFF, 0xFF, TimerForRuntime_Callback, 1, 1);
    Timer_SetInterruptTimeUpdate(TIM_OS_RUNTIME, 10);
    TIM_Cmd(TIM_OS_RUNTIME, ENABLE);
}
