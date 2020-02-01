#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "RCX/Master/RCX.h"
#include "NRF.h"

typedef struct {
    uint8_t Address;
    uint8_t Speed;
    uint8_t Freq;
} NRF_Config_TypeDef;

/*强制单字节对齐*/
#pragma pack (1)
typedef struct {
    union{
        struct{
            uint8_t BT_UP : 1;
            uint8_t BT_DOWN : 1;
            uint8_t BT_BACK : 1;
            uint8_t BT_OK : 1;
            uint8_t BT_L1 : 1;
            uint8_t BT_L2 : 1;
            uint8_t BT_R1 : 1;
            uint8_t BT_R2 : 1;
        }Bit;
        uint8_t Value;
    }Key;
    RCX::Direction_t KnobLimit;
    RCX::Direction_t KnobCab;
    RCX::Axis_t Left;
    RCX::Axis_t Right;
} CTRL_TypeDef;
#pragma pack ()

extern CTRL_TypeDef CTRL;
extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool State_FHSS;
extern bool Enable_CommonPassBack;

extern NRF_Basic nrf;
extern NRF_TRM   nrfTRM;
extern NRF_FHSS  nrfFHSS;

extern NRF_Config_TypeDef NRF_Cfg;
extern uint8_t NRF_AddressConfig[6 * 5];
extern int16_t NRF_SignalStrength;

extern int16_t DR_ST_Value;
extern int16_t DR_TH_Value;

#endif
