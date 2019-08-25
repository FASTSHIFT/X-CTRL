#include "FileGroup.h"

MPU6050 mpu;
Axis_t MPU_Data;

int16_t MPU_Pitch, MPU_Roll, MPU_Null, MPU_Max = MPU_Max_Default;

void Task_MPU6050Read()
{
    mpu.getMotion6(&MPU_Roll, &MPU_Pitch, &MPU_Null, &MPU_Null, &MPU_Null, &MPU_Null);
    LimitValue(MPU_Roll, -MPU_Max, MPU_Max);
    LimitValue(MPU_Pitch, -MPU_Max, MPU_Max);

    MPU_Data.X = map(MPU_Roll, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
	MPU_Data.Y = map(MPU_Pitch, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
}
