#ifndef __RC_PROTOCOL_H
#define	__RC_PROTOCOL_H

#define _RC_PROTOCOL_VERSION "v1.1"

#include "stdint.h"
#include "binary.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CtrlOutput_MaxValue 1000

/********** Enum **********/
typedef enum {
    X_COMMON,
    CAR_DifferentalSteering,
    CAR_ServoSteering
} CtrlObject_t;

typedef enum {
    Pattern_NRF,
    Pattern_USART
} Pattern_t;

typedef enum {
    CtrlMode_Tank,
    CtrlMode_Race,
    CtrlMode_Expert
} CtrlMode_t;

typedef enum {
    BT_UP 	= B00000001,
    BT_DOWN = B00000010,
    BT_LEFT = B00000100,
    BT_RIGHT = B00001000,
    BT_L1	= B00010000,
    BT_L2	= B00100000,
    BT_R1	= B01000000,
    BT_R2	= B10000000
} KeyValue_t;

#define BT_OK  	BT_RIGHT
#define BT_BACK	BT_LEFT

/********** Struct **********/
typedef struct {
    int16_t Min;
    int16_t Mid;
    int16_t Max;
} Calibrat_t;

typedef struct {
    int16_t X;
    int16_t Xmin;
    int16_t Xmid;
    int16_t Xmax;

    int16_t Y;
    int16_t Ymin;
    int16_t Ymid;
    int16_t Ymax;
} Joystick_t;

typedef struct {
    uint8_t CtrlObject;
    uint8_t CtrlMode;
    uint8_t DataPackage_Num;
} RC_Info_t;

typedef struct {
    int16_t X;
    int16_t Y;
} Axis_t;

typedef struct {
    int16_t L;
    int16_t R;
} Direction_t;

/********** Protocol **********/
typedef struct {
    RC_Info_t Info;
    uint8_t Key;
    Direction_t KnobLimit;
    Direction_t KnobCab;

    Direction_t MotorSpeed;
} Protocol_CarDS_t;

typedef struct {
    RC_Info_t Info;
    uint8_t Key;
    Direction_t KnobLimit;
    Direction_t KnobCab;

    int16_t MotorSpeed;
    int16_t SteerAngle;
} Protocol_CarSS_t;

typedef struct {
    RC_Info_t Info;
    uint8_t Key;
    Direction_t KnobLimit;
    Direction_t KnobCab;

    Axis_t Left;
    Axis_t Right;
} Protocol_Common_t;

typedef struct {
    float Channel_1;
    float Channel_2;
    float Channel_3;
    float Channel_4;
    float Channel_5;
    float Channel_6;
    float Channel_7;
    float Channel_8;
} Protocol_Passback_8xChannel_t;

typedef struct {
    uint8_t X;
    uint8_t Y;
    uint8_t TextSize;
    uint16_t TextColor;
    uint16_t BackColor;
    char Text[32 - 3 * sizeof(uint8_t) - 2 * sizeof(uint16_t)];
} Protocol_Passback_CommonDisplay_t;

/********** State **********/
typedef struct {
    bool IsConnect;
    bool IsCorrect;
    uint8_t Pattern;
    uint32_t TimePoint_Recv;
    uint32_t TimePoint_Tran;
} ConnectState_t;

#ifdef __cplusplus
}// extern "C"
#endif

#endif
