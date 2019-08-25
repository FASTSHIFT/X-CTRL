#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RC_Protocol.h"
#include "JW_Protocol.h"

//#define TIM_Handshake TIM4 //握手通信定时中断定时器

#define Master_ID 0x04
#define Master_Description "JW-CTRL"
#define SlaveNum_MAX 5 //从机列表长度

/*************** 广播配置 ***************/
#define BroadcastHead_MasterWait    (uint16_t)0x55AA
#define BroadcastHead_SlaveWait     (uint16_t)0xAA55
#define BroadcastHead_MasterAttach  (uint16_t)0xF0F0
#define BroadcastHead_SlaveAttach   (uint16_t)0x0F0F
#define BroadcastAddr 18,54,98,66,23
#define BroadcastFreq 43
#define BroadcastSpeed nrf.SPEED_250Kbps

extern NRF nrf;
extern uint8_t NRF_TxBuff[32];
extern uint8_t NRF_RxBuff[32];
extern int16_t NRF_SignalStrength;
extern Protocol_Handshake_t HandshakePack_Master;//主机握手数据包
extern Protocol_Handshake_t HandshakePack_Slave[SlaveNum_MAX];//从机握手列表
uint8_t HandshakeRun(uint8_t State, uint8_t SlaveNum = 0, uint8_t Cmd = 0);

extern Protocol_CarDS_t CarDS;
extern Protocol_CarSS_t CarSS;
extern Protocol_Common_t CTRL;
extern Protocol_Common_t COMMON_X;
extern Protocol_Passback_8xChannel_t CarSS_Passback;
extern Protocol_Passback_CommonDisplay_t Common_Passback;
extern ConnectState_t ConnectState;
extern bool Is_CommonPassbackReading;

#endif
