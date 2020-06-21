#include "FileGroup.h"

/*低功耗使能控制*/
bool State_PowerSaving = true;

/**
  * @brief  系统时钟配置
  * @param  无
  * @retval 无
  */
void SysClock_Config()
{
    Init_Value();
    if(State_PowerSaving);
        //SysClock_Init(F_CPU_72MHz);
    else
        SysClock_Init(F_CPU_128MHz);

    Delay_Init();
}
