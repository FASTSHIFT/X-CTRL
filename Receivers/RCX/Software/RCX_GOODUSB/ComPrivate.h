#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RCX.h"

void Com_Init();
void Com_SetDescription(const char* name);

extern NRF_Basic nrf;

extern uint8_t NRF_RxBuff[32];
extern uint8_t NRF_TxBuff[32];

#endif
