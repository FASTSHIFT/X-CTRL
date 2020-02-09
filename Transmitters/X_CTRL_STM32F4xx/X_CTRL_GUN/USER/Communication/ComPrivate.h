#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "RCX/Master/RCX.h"
#include "NRF.h"

typedef struct {
    uint8_t Address;
    uint8_t Speed;
    uint8_t Freq;
} NRF_Config_TypeDef;
extern NRF_Config_TypeDef NRF_Cfg;

extern NRF_Basic nrf;
extern NRF_TRM   nrfTRM;
extern NRF_FHSS  nrfFHSS;

extern uint8_t NRF_AddressConfig[6 * 5];
extern int16_t NRF_SignalStrength;

extern int16_t DR_ST_Value;
extern int16_t DR_TH_Value;

void Com_SetRFEnable(bool en);
bool Com_GetRFEnable();
void Com_SetDefaultChannel();
bool Com_Init();

#endif
