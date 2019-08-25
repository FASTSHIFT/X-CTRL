#include "FileGroup.h"
#include "ComPrivate.h"

#define ServoAngle_MAX 113.5
#define ServoAngle_MIN 77.5

#define Limit_L (RC_X.KnobLimit.L / 1000.0)
#define Limit_R (RC_X.KnobLimit.R / 1000.0)

Servo Steering;
int16_t MotorPwm = 0;
float ServoAngle_MID = (ServoAngle_MAX + ServoAngle_MIN) / 2;
float ServoAngle = ServoAngle_MID;

void Init_MotorServo()
{
    PWM_Init(Motor_Pin, 1000, 400);
    analogWrite(Motor_Pin, 0);

    Steering.attach(SteerServo_Pin);
    Steering.write(ServoAngle);
}

void Task_MID_Setting()
{
    if(RC_X.Key & BT_R2)ServoAngle_MID += 0.5;
    else if(RC_X.Key & BT_L2)ServoAngle_MID -= 0.5;
    __LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);
}

static void NormalCtrlState()
{
    if(RC_X.MotorSpeed >= 50)
    {
        MotorPwm = __Map((RC_X.MotorSpeed * Limit_L), 0, CtrlOutput_MaxValue, 0, 1000);
    }
    else
    {
        MotorPwm = 0;
    }

    if(RC_X.SteerAngle >= 0)
        ServoAngle = __Map((RC_X.SteerAngle * Limit_R), 0, 1000.0, ServoAngle_MID, ServoAngle_MAX);
    else
        ServoAngle = __Map((RC_X.SteerAngle * Limit_R), -1000.0, 0, ServoAngle_MIN, ServoAngle_MID);
}

static void LostCtrlState()
{
    MotorPwm = 0;
    ServoAngle = ServoAngle_MID;
}

void Task_Ctrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
        NormalCtrlState();
    else
        LostCtrlState();

    analogWrite(Motor_Pin, MotorPwm);
    Steering.write(ServoAngle);
}
