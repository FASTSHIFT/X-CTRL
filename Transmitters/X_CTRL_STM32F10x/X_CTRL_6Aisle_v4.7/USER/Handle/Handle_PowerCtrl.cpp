#include "FileGroup.h"

/*低功耗使能控制*/
bool State_PowerSaving = false;

/**
  * @brief  系统时钟配置
  * @param  无
  * @retval 无
  */
void SysClock_Config()
{
    __LoopExecute(Init_Value(),10);
    if(State_PowerSaving);
        //SysClock_Init(F_CPU_72MHz);
    else
        SysClock_Init(F_CPU_128MHz);

    Delay_Init();
}
