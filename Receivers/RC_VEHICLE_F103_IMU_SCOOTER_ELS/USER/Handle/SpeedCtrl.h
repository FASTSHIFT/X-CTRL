#ifndef __SPEEDCTRL_H
#define __SPEEDCTRL_H

#include <stdint.h>

#define MotorSpeed_MAX 120
#define MotorSpeed_MID 65
#define MotorSpeed_MIN 20

extern float MotorSpeed, MotorSpeed_target;
extern float L_limit, R_limit;
extern float ElsSpeed;
extern float ElsDistance;
extern bool EnableSlider;
extern bool IsSpeedUp;

void SpeedUp_Handler();
void Brake_Handler();

#endif
