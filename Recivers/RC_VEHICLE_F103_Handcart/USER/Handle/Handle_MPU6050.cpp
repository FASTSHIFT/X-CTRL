#include "FileGroup.h"
#include "MPU6050.h"
#include "filters.h"
#include "math.h"

/*实例化MPU6050对象*/
static MPU6050 mpu;

float IMUCtrl_Speed = 0;
float IMUCtrl_Pitch = 0;
float IMUCtrl_Yaw = 0;
bool  IMUCtrl_Reset = true;
bool  IMUCtrl_EnablePit = false;
float IMUCtrl_PitError = 20;

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

bool Calibrated = false;
int32_t Gyro_Sum[3];
int32_t Gyro_Times[3];
int32_t Gyro_Offset[3];

/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
void Init_MPU6050()
{
		for(int i = 0; i < 3; i++)
		{
			Gyro_Times[i] = 0;
			Gyro_Sum[i] = 0;
		}
		
    Wire.begin();
		
		delay_ms(300);
		
    __LoopExecute(mpu.initialize(), 100);
    
    mpu.setIntEnabled(true);
    
    pinMode(MPU_INT_Pin, INPUT_PULLUP);
    attachInterrupt(MPU_INT_Pin, Task_MPU6050Read, FALLING);
//    mpu.setClockSource(MPU6050_CLOCK_INTERNAL);
//    mpu.setDLPFMode(0);
//    mpu.setRate(15);
}

PT1Filter gzFilter(0.002, 100);
FIRFilter gyFilter(0.002,  20);
PT1Filter gxFilter(0.002, 100);

/**
  * @brief  MPU6050读取任务
  * @param  无
  * @retval 无
  */
void Task_MPU6050Read()
{  
	int16_t MPU_AX, MPU_AY, MPU_AZ, MPU_GX, MPU_GY, MPU_GZ;
	#define RAD(x) (radians(((double)x)*2000.0/32767.0))
	mpu.getMotion6(&MPU_AX, &MPU_AY, &MPU_AZ, &MPU_GX, &MPU_GY, &MPU_GZ);
	
	int32_t Gyros[] = { MPU_GX, MPU_GY, MPU_GZ };
	
	float valZ = 0;
	float valY = 0;
	
	if(Calibrated)
	{
		for(int i = 0; i < 3; i++)
		{
			Gyros[i] -= Gyro_Offset[i];
		}
		#define PID_OUTLIMIT				60
		#define PID_KI							0.020
		#define PID_KP							4.0
		#define PID_PIT_KI					0.02
		#define PID_PIT_KP					5.0
		
		float ax = RAD(MPU_AX);
		float ay = RAD(MPU_AY);
		float az = RAD(MPU_AZ);
		float gx = RAD(Gyros[0]);
		float gy = RAD(Gyros[1]);
		float gz = RAD(Gyros[2]) + IMUCtrl_Yaw;
		
		float filterGz = gzFilter.Next(gz);
		float filterGy = gyFilter.Next(gy);
		
		static float intgZ = 0;
		static float intgY = 0;
		
		if(IMUCtrl_Reset)
		{
			intgZ = 0;
			intgY = 0;
		}
		else
		{
			intgZ = constrain(intgZ + gz * PID_KI, -PID_OUTLIMIT, PID_OUTLIMIT);
			if(IMUCtrl_EnablePit)
			{
				intgY = intgY + gy * PID_PIT_KI;
			}
			else
			{
				intgY = 170;
			}
		}
		
		float pvalZ = filterGz * PID_KP;
		float pvalY = filterGy * PID_PIT_KP;
		
		if(!IMUCtrl_Reset)
		{
			valZ = intgZ + pvalZ;
			valY = intgY + pvalY;
		}
	}
	else
	{
		for(int i = 0; i < 3; i++)
		{
			Gyro_Times[i] ++;
			Gyro_Sum[i] += Gyros[i];
		}
		if(Gyro_Times[0] >= 1000)
		{
			for(int i = 0; i < 3; i++)
			{
				Gyro_Offset[i] = Gyro_Sum[i] / Gyro_Times[i];
			}
			Calibrated = true;
		}
	}
	
	if(Calibrated)
	{
		float l = -valZ + valY * IMUCtrl_EnablePit + IMUCtrl_Speed + 10 * !IMUCtrl_Reset;
		float r =  valZ + valY * IMUCtrl_EnablePit + IMUCtrl_Speed + 10 * !IMUCtrl_Reset;
		if(l < 60.0) l = 60.0;
		if(r < 60.0) r = 60.0;
		BrushlessMotor_L.write(l);
		BrushlessMotor_R.write(r);
	}
	else
	{
		BrushlessMotor_L.write(60);
		BrushlessMotor_R.write(60);
	}

	//IMUupdate(RAD(MPU_AX), RAD(MPU_AY), RAD(MPU_AZ), RAD(MPU_GX), RAD(MPU_GY), RAD(MPU_GZ));
}
