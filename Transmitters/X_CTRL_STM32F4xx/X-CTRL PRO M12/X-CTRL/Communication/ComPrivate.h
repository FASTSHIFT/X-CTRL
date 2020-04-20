#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "RCX/RCX.h"
#include "NRF/NRF.h"

extern NRF_Basic nrf;
extern NRF_TRM   nrfTRM;
extern NRF_FHSS  nrfFHSS;

void Com_SetEnable(bool en);
bool Com_GetEnable();
void Com_SetDefaultChannel();
bool Com_Init();
void Com_Update();

typedef enum{
    PBS_Setup,
    PBS_Loop,
    PBS_Error,
    PBS_Exit
}PassbackState_Type;
void Com_PassbackProcess(uint8_t state);
void Passback_XBox360Sim(uint8_t state);

#endif
