/** @Author: _VIFEXTech
  * @Describe: 通用的遥控协议
  * @Finished: UNKNOWTIME - v1.0 能用了
  * @Upgrade:  UNKNOWTIME - v1.1 改了一些结构体
  * @Upgrade:  2018.12.25 - v1.2 去除TimePoint_Tran, 添加握手协议与时钟基准
  * @Upgrade:  2018.12.26 - v1.3 握手协议调试成功
  * @Upgrade:  2019.2.13  - v1.4 使用单字节对齐，保证跨平台兼容性
  * @Upgrade:  2020.1.20  - v1.5 添加跳频功能 
  */

#ifndef __RC_PROTOCOL_H
#define __RC_PROTOCOL_H

#define _RC_PROTOCOL_VERSION "v1.5"

#include "stdint.h"
//#include "binary.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CtrlOutput_MaxValue 1000
#pragma pack (1)

/********** Enum **********/
typedef enum
{
    X_COMMON,
    CAR_DifferentalSteering,
    CAR_ServoSteering,
    CtrlObject_MAX
} CtrlObject_Type;

typedef enum
{
    Pattern_NRF,
    Pattern_USART,
    Pattern_MAX
} Pattern_Type;

typedef enum
{
    CtrlMode_Tank,
    CtrlMode_Race,
    CtrlMode_Expert,
    CtrlMode_MAX
} CtrlMode_Type;

typedef enum
{
    BT_UP    = B00000001,
    BT_DOWN  = B00000010,
    BT_LEFT  = B00000100,
    BT_RIGHT = B00001000,
    BT_L1    = B00010000,
    BT_L2    = B00100000,
    BT_R1    = B01000000,
    BT_R2    = B10000000
} KeyValue_t;
#define BT_OK   BT_RIGHT
#define BT_BACK BT_LEFT

typedef enum
{
    CMD_AgreeConnect = 1,
    CMD_AttachConnect,
    CMD_Disconnect,
    CMD_ReqConnect
} Commond_Type;

typedef enum
{
    HandshakeState_Prepare,
    HandshakeState_Search,
    HandshakeState_ReqConnect,
    HandshakeState_ReConnect,
    HandshakeState_Connected
} HandshakeState_Type;

/********** Basic Struct **********/
typedef struct
{
    int16_t Min;
    int16_t Mid;
    int16_t Max;
} Calibrat_t;

typedef struct
{
    int16_t X;
    int16_t Xmin;
    int16_t Xmid;
    int16_t Xmax;

    int16_t Y;
    int16_t Ymin;
    int16_t Ymid;
    int16_t Ymax;
} Joystick_t;

typedef struct
{
    uint8_t CtrlObject;
    uint8_t CtrlMode;
    uint8_t DataPackage_Num;
} RC_Info_t;

typedef struct
{
    int16_t X;
    int16_t Y;
} Axis_t;

typedef struct
{
    int16_t L;
    int16_t R;
} Direction_t;

typedef struct{
    bool Passback : 1;
    bool FHSS     : 1;
    uint8_t Reserved : 6;
}FunctionEnable_t;

/********** Protocol Struct **********/
#define FreqList_Length 10
typedef struct
{
    uint16_t HeadCode;
    uint8_t ID;
    char Description[10];
    uint8_t CtrlType;
    FunctionEnable_t EnableFunction;
    uint8_t Commond;

    uint8_t Speed;
    uint8_t Address[5];
    uint8_t FerqList[FreqList_Length];
} Protocol_Handshake_t;

typedef struct
{
    RC_Info_t Info;
    uint8_t Key;
    Direction_t KnobLimit;
    Direction_t KnobCab;

    Direction_t MotorSpeed;

    uint32_t ReferenceClock;
} Protocol_CarDS_t;

typedef struct
{
    RC_Info_t Info;
    uint8_t Key;
    Direction_t KnobLimit;
    Direction_t KnobCab;

    int16_t MotorSpeed;
    int16_t SteerAngle;

    uint32_t ReferenceClock;
} Protocol_CarSS_t;

typedef struct
{
    RC_Info_t Info;
    uint8_t Key;
    Direction_t KnobLimit;
    Direction_t KnobCab;

    Axis_t Left;
    Axis_t Right;

    uint32_t ReferenceClock;
} Protocol_Common_t;

typedef struct
{
    uint16_t HeadCode;
    uint8_t Type;
    uint8_t ID;
    float Channel_1;
    float Channel_2;
    float Channel_3;
    float Channel_4;
    float Channel_5;
    float Channel_6;
    float Channel_7;
} Protocol_Passback_xChannel_t;

typedef struct
{
    uint8_t X;
    uint8_t Y;
    uint8_t TextSize;
    uint16_t TextColor;
    uint16_t BackColor;
    char Text[32 - 3 * sizeof(uint8_t) - 2 * sizeof(uint16_t)];
} Protocol_Passback_CommonDisplay_t;

/********** State **********/
typedef struct
{
    bool IsConnect;
    bool IsCorrect;
    uint8_t Pattern;
    uint32_t TimePoint_Recv;
} ConnectState_t;

#pragma pack ()

#ifdef __cplusplus
}// extern "C"
#endif

#endif
