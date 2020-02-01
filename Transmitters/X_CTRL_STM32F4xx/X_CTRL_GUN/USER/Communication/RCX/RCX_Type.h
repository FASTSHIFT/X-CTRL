/** @Author: _VIFEXTech
  * @Describe: 通用的遥控协议
  * @Finished: UNKNOWTIME - v1.0 能用了
  * @Upgrade:  UNKNOWTIME - v1.1 改了一些结构体
  * @Upgrade:  2018.12.25 - v1.2 去除TimePoint_Tran, 添加握手协议与时钟基准
  * @Upgrade:  2018.12.26 - v1.3 握手协议调试成功
  * @Upgrade:  2019.2.13  - v1.4 使用单字节对齐，保证跨平台兼容性
  * @Upgrade:  2020.1.20  - v1.5 添加跳频功能
  * @Upgrade:  2020.1.30  - v1.6 协议层重构，新协议命名为RCX，兼容旧协议
  */

#ifndef __RCX_TYPE_H
#define __RCX_TYPE_H

#include "stdint.h"

/*通道数定义*/
#define RCX_CHANNEL_NUM 8

#if( RCX_CHANNEL_NUM <= 0 || RCX_CHANNEL_NUM > 12 )
#  error "RCX_CHANNEL_NUM must >0 and <= 12"
#endif

namespace RCX
{

/*强制单字节对齐*/
#pragma pack (1)

/*控制对象类型*/
typedef enum
{
    X_COMMON,
    CAR_DifferentalSteering,
    CAR_ServoSteering,
    AIRCRAFT_FixedWing,
    AIRCRAFT_QuadRotor,
    CtrlObject_MAX
} CtrlObject_Type;

/*校准*/
typedef struct
{
    int16_t Min;
    int16_t Mid;
    int16_t Max;
} Calibrat_t;

/*摇杆*/
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

/*坐标*/
typedef struct
{
    int16_t X;
    int16_t Y;
} Axis_t;

/*方向*/
typedef struct
{
    int16_t L;
    int16_t R;
} Direction_t;

/*通用通信格式 (32 Bytes)*/
typedef struct
{
    /*数据包头 3 Bytes*/
    struct
    {
        uint8_t ObjectType;
        uint8_t ObjectID;
        uint8_t HeartBeat;
    } Head;

    /*按键 1 Byte*/
    uint8_t Key;

    /*通道 RCX_CHANNEL_NUM * 2 Bytes*/
    int16_t Channel[RCX_CHANNEL_NUM];
    
    /*保留*/
    uint8_t RESERVED[27 - (RCX_CHANNEL_NUM * 2)];

    /*CRC校验码 1 Byte*/
    uint8_t CRC8;
} PackCommon_t;

#pragma pack ()

}/*end of namespace RCX*/

#endif
