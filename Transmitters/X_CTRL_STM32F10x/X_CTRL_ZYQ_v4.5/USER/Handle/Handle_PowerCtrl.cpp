#include "FileGroup.h"

bool State_PowerSaving = false;

void SysClock_Config()
{
    Init_Value();
    if(State_PowerSaving);
        //SysClock_Init(F_CPU_72MHz);
    else
        SysClock_Init(F_CPU_88MHz);

    Delay_Init();
}
