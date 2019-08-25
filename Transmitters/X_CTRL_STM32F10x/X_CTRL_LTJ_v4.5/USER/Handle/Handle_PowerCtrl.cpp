#include "FileGroup.h"

bool State_PowerSaving = false;

void SysClock_Config()
{
    __LoopExecute(Init_Value(),2);
    if(State_PowerSaving);
        //SysClock_Init(F_CPU_72MHz);
    else
        SysClock_Init(F_CPU_128MHz);

    Delay_Init();
}
