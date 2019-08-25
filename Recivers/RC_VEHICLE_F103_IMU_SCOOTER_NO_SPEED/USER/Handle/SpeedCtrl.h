#ifndef __SPEEDCTRL_H
#define __SPEEDCTRL_H

#include <stdint.h>

#define MotorSpeed_MAX 120
#define MotorSpeed_MID 60
#define MotorSpeed_MIN 20

extern float MotorSpeed, MotorSpeed_target;
extern float L_limit, R_limit;
extern bool EnableSlider;
extern bool IsSpeedUp;
extern float ElsSpeed;
extern float ElsDistance;

void SpeedUp_Handler();
void Brake_Handler();

#endif
