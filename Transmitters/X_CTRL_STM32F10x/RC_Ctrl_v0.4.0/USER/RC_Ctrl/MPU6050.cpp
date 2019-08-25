#include "FileGroup.h"

#ifdef USE_MPU6050
void Read_MPU6050()
{
    mpu.getMotion6(&MPU_Pitch, &MPU_Roll, &MPU_Null, &MPU_Null, &MPU_Null, &MPU_Null);
    Limit(MPU_Roll, -MPU_MAX, MPU_MAX);
    Limit(MPU_Pitch, -MPU_MAX, MPU_MAX);
}
#endif

