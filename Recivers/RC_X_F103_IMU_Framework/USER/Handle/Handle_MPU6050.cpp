#include "FileGroup.h"
#include "MPU6050.h"
#include "filters.h"
#include "math.h"

/*实例化MPU6050对象*/
static MPU6050 mpu;

int16_t MPU_AX, MPU_AY, MPU_AZ, MPU_GX, MPU_GY, MPU_GZ;

/**
  * @brief  MPU6050初始化
  * @param  无
  * @retval 无
  */
void Init_MPU6050()
{		
    Wire.begin();
		
    __LoopExecute(mpu.initialize(), 100);
    
    mpu.setIntEnabled(true);
    
    pinMode(MPU_INT_Pin, INPUT_PULLUP);
    attachInterrupt(MPU_INT_Pin, Task_MPU6050Read, RISING);
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
	#define RAD(x) (radians(((double)x)*2000.0/32767.0))
	mpu.getMotion6(&MPU_AX, &MPU_AY, &MPU_AZ, &MPU_GX, &MPU_GY, &MPU_GZ);
}
