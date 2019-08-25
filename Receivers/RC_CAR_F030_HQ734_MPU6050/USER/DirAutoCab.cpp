#include "FileGroup.h"

MPU6050 mpu;
int16_t Gyro_Z;
int16_t MPU_Null;
double ServoAngle_CAB;
double ServoPID_P = 0;
double ServoPID_I = 0.01;
double ServoPID_D = 0;
double ServoPID_LastError = 0;
double ServoPID_PrevError = 0;
void Task_DirAutoCab()
{
    int16_t Gyro_Z_Temp;
    mpu.getMotion6(&MPU_Null, &MPU_Null, &MPU_Null, &MPU_Null, &MPU_Null, &Gyro_Z_Temp);
    Gyro_Z = Gyro_Z_Temp * 0.01;
	
    double Gyro_target = -(RC_CAR.SteerAngle * (RC_CAR.KnobLimit.R / 1000.0)) / 10.0;
	double angleError = Gyro_target - Gyro_Z;
	if(!ServoPID_LastError)
	{
		ServoPID_LastError = ServoPID_PrevError = angleError;
	}
    ServoAngle_CAB += ServoPID_I * angleError + ServoPID_P * (angleError - ServoPID_LastError) + ServoPID_D * (angleError - 2 * ServoPID_LastError - ServoPID_PrevError);
	ServoPID_PrevError = ServoPID_LastError;
	ServoPID_LastError = angleError;
    LimitValue(ServoAngle_CAB, ServoAngle_MIN, ServoAngle_MAX);
    ServoAngle = ServoAngle_MID = ServoAngle_CAB;
}
