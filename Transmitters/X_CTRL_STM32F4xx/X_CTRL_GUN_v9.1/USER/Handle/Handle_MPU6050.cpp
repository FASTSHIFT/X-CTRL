#include "FileGroup.h"
#include "ComPrivate.h"
#include "MPU6050.h"
#include "math.h"

/*实例化MPU6050对象*/
static MPU6050 mpu;

/*MPU使能控制*/
bool State_MPU = OFF;

/*MPU最大角度限幅*/
#define MPU_Max_Default 60

/*MPU角度数据*/
Axis_t MPU_Data;

/*姿态解算数据配置*/
#define Kp 1.0f
#define Ki 0.01f
#define halfT 0.01f
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
static float exInt = 0, eyInt = 0, ezInt = 0;
float Yaw = 0, Pitch = 0, Roll = 0;

/**
  * @brief  姿态解算
  * @param  
  * @retval 无
  */
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

    Pitch  = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f;
    Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f;
    //Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
}

/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
static void Init_MPU6050()
{
    Wire.begin();
    __LoopExecute(mpu.initialize(), 100);
//    mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
//    mpu.setDLPFMode(0);
//    mpu.setRate(15);
}

/**
  * @brief  MPU6050读取任务
  * @param  无
  * @retval 无
  */
void Task_MPU6050Read()
{
    if(!State_MPU)
        return;
    
    __ExecuteOnce(Init_MPU6050());
    
    int16_t ax, ay, az, gx, gy, gz, max = MPU_Max_Default;
    #define RAD(x) (radians(((double)x)*2000.0/32767.0))
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    IMUupdate(RAD(ay), RAD(ax), RAD(-az), RAD(gy), RAD(gx), RAD(-gz));

    MPU_Data.X = fmap(Pitch, -max, max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    MPU_Data.Y = -fmap(Roll, -max, max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    __LimitValue(MPU_Data.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    __LimitValue(MPU_Data.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    
    /*Page_SetGravity.cpp*/
    /*将MPU所产生的数据替代摇杆值*/
    extern int16_t *IMU_Data[4];
    if(IMU_Data[0]) 
        CTRL.Left.X = *IMU_Data[0];
    else
        CTRL.Left.X = JS_L.X;
    
    if(IMU_Data[1])
        CTRL.Left.Y = *IMU_Data[1];
    else
        CTRL.Left.Y = JS_L.Y;
    
    if(IMU_Data[2])
        CTRL.Right.X = *IMU_Data[2];
    else
        CTRL.Right.X = JS_R.X;
    
    if(IMU_Data[3])
        CTRL.Right.Y = *IMU_Data[3];
    else
        CTRL.Right.Y = JS_R.Y;
}
