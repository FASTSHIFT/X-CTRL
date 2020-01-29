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
float Yaw = 0, Pitch = 0, Roll = 0;
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
static float exInt = 0, eyInt = 0, ezInt = 0;

/**
  * @brief  姿态解算
  * @param
  * @retval 无
  */
static void IMUupdate(float ax, float ay, float az, float gx, float gy, float gz)
{
    /*姿态解算数据*/
    const double Kp = 1.0f;
    const double Ki = 0.01f;
    const double halfT = 0.01f;
    
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
    Yaw = atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3f;
}

bool IsCalibrateStart = false;
static uint32_t CalibrateStartTime = 0;
static uint16_t CalibrateCnt;
static int32_t gx_sum = 0,gy_sum = 0,gz_sum = 0;

void IMU_CalibrateStart()
{
    if(IsCalibrateStart || !State_MPU)
        return;
    
    IsCalibrateStart = true;
    CalibrateStartTime = millis();
    gx_sum = 0,gy_sum = 0,gz_sum = 0;
    CalibrateCnt = 0;
}

static void IMU_Calibrate(int16_t* gx,int16_t* gy,int16_t* gz)
{
    static int16_t gx_cab = 0,gy_cab = 0,gz_cab = 0;
    if(IsCalibrateStart)
    {
        gx_sum += *gx;
        gy_sum += *gy;
        gz_sum += *gz;
        CalibrateCnt++;
        if(millis() - CalibrateStartTime >= 2000)
        {
            gx_cab = gx_sum / CalibrateCnt;
            gy_cab = gy_sum / CalibrateCnt;
            gz_cab = gz_sum / CalibrateCnt;
            
            q0 = 1, q1 = 0, q2 = 0, q3 = 0;
            exInt = 0, eyInt = 0, ezInt = 0;
            IsCalibrateStart = false;
        }
    }
    else
    {
        *gx -= gx_cab;
        *gy -= gy_cab;
        *gz -= gz_cab;
    }
}

/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
static void Init_MPU6050()
{
    Wire.begin();
    mpu.initialize();
//    mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
//    mpu.setDLPFMode(0);
//    mpu.setRate(15);
}

static void MPU6050_WriteChannel()
{
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
#define RAD(x) (radians(((double)x)*2000.0f/32767.0f))
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    IMU_Calibrate(&gx, &gy, &gz);
//    IMUupdate(RAD(ay), RAD(ax), RAD(-az), RAD(gy), RAD(gx), RAD(-gz));
    IMUupdate(RAD(-az), RAD(ay), RAD(ax), RAD(-gz), RAD(gy), RAD(gx));

    static int last_x, last_y;
    int now_x = -fmap(Yaw, -max, max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    int now_y = -fmap(Roll, -max, max, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    MPU_Data.X = now_x - last_x;
    MPU_Data.Y = now_y - last_y;
    last_x = now_x;
    last_y = now_y;
    __LimitValue(MPU_Data.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    __LimitValue(MPU_Data.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);

    MPU6050_WriteChannel();
}
