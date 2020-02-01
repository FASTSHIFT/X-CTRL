#ifndef __IMU_PRIVATE_H
#define __IMU_PRIVATE_H

#include <stdint.h>

typedef struct{
    float Angle;
    float Limit;
}IMU_Angle_TypeDef;

typedef struct{
    IMU_Angle_TypeDef Pitch;
    IMU_Angle_TypeDef Roll;
    IMU_Angle_TypeDef Yaw;
}IMU_Axis_TypeDef;

typedef struct{
    int16_t Data;
    int16_t Last;
    int16_t delta;
    bool Reverse;
}IMU_ChBasic_TypeDef;

typedef struct{
    IMU_ChBasic_TypeDef Pitch;
    IMU_ChBasic_TypeDef Roll;
    IMU_ChBasic_TypeDef Yaw;
}IMU_Channel_TypeDef;

extern IMU_Axis_TypeDef     IMU_Axis;
extern IMU_Channel_TypeDef  IMU_Channel;
extern bool IsCalibrateStart;

void IMU_NormReset();
void IMU_CalibrateStart();

#endif
