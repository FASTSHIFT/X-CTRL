#ifndef __RC_PROTOCOL_H
#define	__RC_PROTOCOL_H

#include "stdint.h"
#include "binary.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint8_t CtrlObject;
	uint8_t DataPackage_Num;
}RC_Info_t;

typedef struct{
	int16_t X;
	int16_t Y;
}Axis_t;

typedef struct{
	int16_t L;
	int16_t R;
}Direction_t;

typedef struct{
	RC_Info_t Info;
	Direction_t MotorSpeed;
	Direction_t Knob;
	uint8_t Key;
}Protocol_CarDS_t;

typedef struct{
	RC_Info_t Info;
	int16_t MotorSpeed;
	int16_t SteerAngle;
	Direction_t Knob;
	uint8_t Key;
}Protocol_CarSS_t;

typedef struct{
	RC_Info_t Info;
	Axis_t Left;
	Axis_t Right;
	Direction_t Knob;
	uint8_t Key;
}Protocol_Common_t;

typedef struct{
	bool IsConnect;
	bool IsCorrect;
	uint8_t Pattern;
	uint32_t TimePoint_Recv;
	uint32_t TimePoint_Tran;
}ConnectState_t;

typedef enum{
	BT_UP 	= B00000001,
	BT_DOWN = B00000010,
	BT_LEFT = B00000100,
	BT_RIGHT= B00001000,
	BT_L1	= B00010000,
	BT_L2	= B00100000,
	BT_R1	= B01000000,
	BT_R2	= B10000000
}KeyValue_t;

typedef enum{
	CAR_DifferentalSteering,
	CAR_ServoSteering,
	COMMON
}CtrlObject_t;

#ifdef __cplusplus
}// extern "C"
#endif

#endif
