#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RC_Protocol.h"
#include "Config.h"


#define Master_ID 0x03
#define Master_Description "X-CTRL"
#define SlaveNum_MAX 5 //从机列表长度
#define NRF_AddressNum 6

/*************** 广播配置 ***************/
#define BroadcastHead_MasterWait    (uint16_t)0x55AA
#define BroadcastHead_SlaveWait     (uint16_t)0xAA55
#define BroadcastHead_MasterAttach  (uint16_t)0xF0F0
#define BroadcastHead_SlaveAttach   (uint16_t)0x0F0F
#define BroadcastAddr 18,54,98,66,23
#define BroadcastFreq 43
#define BroadcastSpeed nrf.SPEED_250Kbps

typedef struct {
    uint8_t Address;
    uint8_t Speed;
    uint8_t Freq;
} NRF_Config_TypeDef;

typedef void(*CustomDataPack_Callback_t)(uint8_t*);

extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool Enable_CommonPassBack;

extern NRF nrf;
extern uint8_t NRF_TxBuff[32];
extern uint8_t NRF_RxBuff[32];
extern Protocol_Handshake_t HandshakePack_Master;//主机握手数据包
extern Protocol_Handshake_t HandshakePack_Slave[SlaveNum_MAX];//从机握手列表
extern bool Lock_CommonPassback;
extern NRF_Config_TypeDef NRF_Cfg;
extern uint8_t NRF_AddressConfig[NRF_AddressNum * 5];
extern int16_t NRF_SignalStrength;
extern ConnectState_t ConnectState;
extern Protocol_CarDS_t CarDS;
extern Protocol_CarSS_t CarSS;
extern Protocol_Common_t CTRL;
extern Protocol_Passback_xChannel_t xChannel_Passback;
extern Protocol_Passback_CommonDisplay_t Common_Passback;

uint8_t HandshakeRun(uint8_t State, uint8_t SlaveNum = 0, uint8_t Cmd = 0);

void SetUserCustomDataPack_Callback(CustomDataPack_Callback_t callback);
bool AddUserCustomPassBack_Callback(CustomDataPack_Callback_t callback);

#endif
