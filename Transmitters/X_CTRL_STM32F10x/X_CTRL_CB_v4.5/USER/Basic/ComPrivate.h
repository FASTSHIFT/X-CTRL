#ifndef __COMPRIVATE_H
#define __COMPRIVATE_H

#include "NRF.h"
#include "RC_Protocol.h"

#define TIM_Handshake TIM4 //握手通信定时中断定时器

#define Master_ID 0x03
#define Master_Description "X-CTRL"
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
extern Protocol_Handshake_t HandshakePack_Master;//主机握手数据包
extern Protocol_Handshake_t HandshakePack_Slave[SlaveNum_MAX];//从机握手列表
extern bool Is_CommonPassbackReading;
uint8_t HandshakeRun(uint8_t State, uint8_t SlaveNum = 0, uint8_t Cmd = 0);

#endif
