#include "FileGroup.h"

Servo SteerServo;
Servo BrushlessMotor;
uint16_t ServoAngle, ServoAngle_MID = (ServoAngle_MIN + ServoAngle_MAX) / 2;
uint16_t MotorSpeed, MotorSpeed_MID = 60;

void Task_MID_Setting()
{
}

void Task_Ctrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
    }
    else
    {
    }
}
