#include "FileGroup.h"

MPU6050 mpu;
Axis_t MPU_Data;
bool MPU_State = OFF;

int16_t MPU_AX, MPU_AY, MPU_AZ, MPU_GX, MPU_GY, MPU_GZ, MPU_Max = MPU_Max_Default;

#define Kp 1.0f
#define Ki 0.01f
#define halfT 0.01f
float q[4] = {1, 0, 0, 0};
float exInt = 0, eyInt = 0, ezInt = 0;
float Yaw = 0, Pitch = 0, Roll = 0;
void IMUupdate(float ax, float ay, float az, float gx, float gy, float gz)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    norm = sqrt(ax * ax + ay * ay + az * az);
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    vx = 2 * (q[1] * q[3] - q[0] * q[2]);
    vy = 2 * (q[0] * q[1] + q[2] * q[3]);
    vz = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];

    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    exInt = exInt + ex * Ki;
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;

    q[0] = q[0] + (-q[1] * gx - q[2] * gy - q[3] * gz) * halfT;
    q[1] = q[1] + (q[0] * gx + q[2] * gz - q[3] * gy) * halfT;
    q[2] = q[2] + (q[0] * gy - q[1] * gz + q[3] * gx) * halfT;
    q[3] = q[3] + (q[0] * gz + q[1] * gy - q[2] * gx) * halfT;

    norm = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    q[0] = q[0] / norm;
    q[1] = q[1] / norm;
    q[2] = q[2] / norm;
    q[3] = q[3] / norm;

    Pitch  = asin(-2 * q[1] * q[3] + 2 * q[0] * q[2]) * 57.3;
    Roll = atan2(2 * q[2] * q[3] + 2 * q[0] * q[1], -2 * q[1] * q[1] - 2 * q[2] * q[2] + 1) * 57.3;
    //Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
}

#define RAD(x) (radians(((double)x)*2000.0/32767.0))

void Task_MPU6050Read()
{
    mpu.getMotion6(&MPU_AX, &MPU_AY, &MPU_AZ, &MPU_GX, &MPU_GY, &MPU_GZ);
    IMUupdate(RAD(MPU_AX), RAD(MPU_AY), RAD(MPU_AZ), RAD(MPU_GX), RAD(MPU_GY), RAD(MPU_GZ));

    MPU_Data.Y = -fmap(Pitch, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    MPU_Data.X = -fmap(Roll, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    LimitValue(MPU_Data.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    LimitValue(MPU_Data.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);

//    MPU_Data.X = map(MPU_AX, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
//    MPU_Data.Y = map(MPU_AY, -MPU_Max, MPU_Max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
}
