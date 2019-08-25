#include "FileGroup.h"
#include "MPU6050.h"

#define MPU_Max_Default 60

MPU6050 mpu;
Axis_t MPU_Data;
bool State_MPU = OFF;
extern bool IS_ScreenBusy;

int16_t MPU_AX, MPU_AY, MPU_AZ, MPU_GX, MPU_GY, MPU_GZ, MPU_Max = MPU_Max_Default;

#define Kp 1.0f
#define Ki 0.01f
#define halfT 0.01f
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
static float exInt = 0, eyInt = 0, ezInt = 0;
float Yaw = 0, Pitch = 0, Roll = 0;
static void IMUupdate(float ax, float ay, float az, float gx, float gy, float gz)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    norm = sqrt(ax * ax + ay * ay + az * az);
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    vx = 2 * (q1 * q3 - q0 * q2);
    vy = 2 * (q0 * q1 + q2 * q3);
    vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    exInt = exInt + ex * Ki;
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;

    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

    Pitch  = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3;
    Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3;
    //Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
}

void Init_MPU6050()
{
    Wire.begin();
    __LoopExecute(mpu.initialize(), 100);
//    mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
//    mpu.setDLPFMode(0);
//    mpu.setRate(15);
}

void Task_MPU6050Read()
{
    if(!State_MPU || IS_ScreenBusy) return;
    #define RAD(x) (radians(((double)x)*2000.0/32767.0))
    mpu.getMotion6(&MPU_AX, &MPU_AY, &MPU_AZ, &MPU_GX, &MPU_GY, &MPU_GZ);
    IMUupdate(RAD(MPU_AX), RAD(MPU_AY), RAD(MPU_AZ), RAD(MPU_GX), RAD(MPU_GY), RAD(MPU_GZ));

    MPU_Data.X = -fmap(Roll, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    MPU_Data.Y = -fmap(Pitch, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);

    __LimitValue(MPU_Data.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    __LimitValue(MPU_Data.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);

//    MPU_Data.X = map(MPU_AX, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
//    MPU_Data.Y = map(MPU_AY, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
}
