#ifndef __RCX_HANDSHAKE_H
#define __RCX_HANDSHAKE_H

#include "stdint.h"

/*广播配置*/
#define RCX_BROADCAST_ADDR  18,54,98,66,23
#define RCX_BROADCAST_FREQ  43/*2443MHz*/
#define RCX_BROADCAST_SPEED 0 /*SPEED_250Kbps*/

namespace RCX
{
namespace Handshake
{

/*握手命令*/
typedef enum
{
    CMD_None,
    CMD_AgreeConnect,    /*同意连接*/
    CMD_AttachConnect,   /*绑定连接*/
    CMD_Disconnect,      /*断开连接*/
    CMD_ReqConnect,      /*请求连接*/
    CMD_MAX
} CMD_Type;

/*握手状态机*/
typedef enum
{
    State_Idle,        /*空闲*/
    State_Prepare,     /*准备*/
    State_Search,      /*搜索*/
    State_ReqConnect,  /*请求连接*/
    State_ReConnect,   /*重新连接*/
    State_Connected,   /*连接成功*/
    State_MAX
} State_Type;

/*广播头*/
typedef enum
{
    BroadcastHead_MasterWait   = (uint16_t)0x55AA,
    BroadcastHead_SlaveWait    = (uint16_t)0xAA55,
    BroadcastHead_MasterAttach = (uint16_t)0xF0F0,
    BroadcastHead_SlaveAttach  = (uint16_t)0x0F0F
}BroadcastHead_Type;

/*强制单字节对齐*/
#pragma pack (1)

/*握手数据包格式 32 Bytes*/
typedef struct
{
    /*广播帧头 2 Bytes*/
    uint16_t BroadcastHead;
    
    /*ID号 1 Byte*/
    uint8_t ID;
    
    /*设备描述 10 Bytes*/
    char Description[10];
    
    /*控制对象 1 Byte*/
    uint8_t Type;

    /*额外功能使能 1 Byte*/
    struct
    {
        /*回传使能*/
        bool Passback : 1;
        /*跳频使能*/
        bool FHSS     : 1;
        /*保留 6 bits*/
        uint8_t RESERVED : 6;
    } EnableFunction;

    /*命令 1 Byte*/
    uint8_t CMD;/*CMD_Type*/

    /*通信速度 1 Byte*/
    uint8_t Speed;
    
    /*通信地址 5 Bytes*/
    uint8_t Address[5];
    
    /*跳频表 10 Bytes*/
    uint8_t FerqList[10];
} Pack_t;

#pragma pack ()

}
}

#endif
