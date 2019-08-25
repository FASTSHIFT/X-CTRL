#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RC_Protocol.h"

typedef Protocol_Common_t Protocol_CtrlType;
typedef struct {
    uint8_t Address[5];
    uint8_t Speed;
    uint8_t Freq;
} NRF_Config_TypeDef;

extern NRF nrf;
extern NRF_Config_TypeDef NRF_Cfg;
extern uint8_t NRF_TxBuff[32];
extern uint8_t NRF_RxBuff[32];
extern Protocol_Handshake_t HandshakePack_Attached;
extern Protocol_CtrlType RC_X;
extern ConnectState_t ConnectState;
extern bool PassBack_State;

/*************** 从机配置 ***************/
#define Slave_ID 0xAA
#define Slave_Description "-ELS-"
#define Slave_Type X_COMMON
#define Slave_PassbackSupport true

/*************** 广播配置 ***************/
#define BroadcastHead_MasterWait    (uint16_t)0x55AA
#define BroadcastHead_SlaveWait     (uint16_t)0xAA55
#define BroadcastHead_MasterAttach  (uint16_t)0xF0F0
#define BroadcastHead_SlaveAttach   (uint16_t)0x0F0F
#define BroadcastAddr 18,54,98,66,23
#define BroadcastFreq 43
#define BroadcastSpeed nrf.SPEED_250Kbps

#endif
