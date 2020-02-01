#include "FileGroup.h"
#include "ComPrivate.h"
#include "MPU6050.h"
#include "math.h"
#include "IMU_Private.h"

#define IMU_RAD(x) (radians(((double)x)*2000.0f/32767.0f))

/*实例化MPU6050对象*/
static MPU6050 mpu;

/*MPU使能控制*/
bool State_MPU = OFF;

/*MPU角度数据*/
IMU_Axis_TypeDef    IMU_Axis;
IMU_Channel_TypeDef IMU_Channel;
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
static float exInt = 0, eyInt = 0, ezInt = 0;

void IMU_NormReset()
{
    q0 = 1, q1 = 0, q2 = 0, q3 = 0;
    exInt = 0, eyInt = 0, ezInt = 0;
}

/**
  * @brief  姿态解算
  * @param
  * @retval 无
  */
static void IMU_NormUpdate(float ax, float ay, float az, float gx, float gy, float gz)
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

    IMU_Axis.Pitch.Angle = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f;
    IMU_Axis.Roll.Angle  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f;
    IMU_Axis.Yaw.Angle   = -atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3f;
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
            
            IMU_NormReset();
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

static void IMU_AngleToChannel(IMU_Angle_TypeDef angle, IMU_ChBasic_TypeDef* ch)
{
    float agl = constrain(angle.Angle, -angle.Limit, angle.Limit);
    ch->Data = (ch->Reverse ? -1 : 1) * __Map(agl, -angle.Limit, angle.Limit, -RCX_ChannelData_Max, RCX_ChannelData_Max);
    ch->delta = ch->Data - ch->Last;
    ch->Last = ch->Data;
}

static void IMU_ChannelUpdate()
{
    IMU_AngleToChannel(IMU_Axis.Pitch, &IMU_Channel.Pitch);
    IMU_AngleToChannel(IMU_Axis.Roll,  &IMU_Channel.Roll);
    IMU_AngleToChannel(IMU_Axis.Yaw,   &IMU_Channel.Yaw);
}

/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
static void Init_IMU()
{
    Wire.begin();
    __LoopExecute(mpu.initialize(), 50);
//    mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
//    mpu.setDLPFMode(0);
//    mpu.setRate(15);
    
    IMU_Axis.Pitch.Limit = 90;
    IMU_Axis.Roll.Limit = 90;
    IMU_Axis.Yaw.Limit = 180;
    
    IMU_Channel.Roll.Reverse = true;
}

/**
  * @brief  IMU处理任务
  * @param  无
  * @retval 无
  */
void Task_IMU_Process()
{
    if(!State_MPU)
        return;

    __ExecuteOnce(Init_IMU());

    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    IMU_Calibrate(&gx, &gy, &gz);
    IMU_NormUpdate(IMU_RAD(-az), IMU_RAD(ay), IMU_RAD(ax), IMU_RAD(-gz), IMU_RAD(gy), IMU_RAD(gx));
    IMU_ChannelUpdate();
}
