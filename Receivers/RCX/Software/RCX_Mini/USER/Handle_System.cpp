#include "FileGroup.h"

void IWDG_Init(uint8_t prescaler, uint16_t reload)
{
    /*Tout=((4 * 2 ^ prescaler) * reload)/32 (ms)*/
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
    IWDG_SetPrescaler(prescaler); //设置IWDG分频系数
    IWDG_SetReload(reload);   //设置IWDG装载值
    IWDG_ReloadCounter(); //reload
    IWDG_Enable();       //使能看门狗
}
