#include "FileGroup.h"
#include "ComPrivate.h"
#include "Servo.h"

#define ServoAngle_MAX 106
#define ServoAngle_MIN 48
#define MotorSpeed_MAX 160
#define MotorSpeed_MIN 40

Servo SteerServo;
Servo BrushlessMotor;
uint16_t ServoAngle, ServoAngle_MID = (ServoAngle_MIN + ServoAngle_MAX) / 2;
uint16_t MotorSpeed, MotorSpeed_MID = 60;

void Init_MotorServo()
{
    BrushlessMotor.attach(Motor_Pin);
    BrushlessMotor.write(MotorSpeed_MID);
    SteerServo.attach(Servo_Pin);
    SteerServo.write(ServoAngle_MID);
}

void Task_MID_Setting()
{
    if(RC_X.Key & BT_R2)ServoAngle_MID--;
    else if(RC_X.Key & BT_L2)ServoAngle_MID++;
    __LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);
}

static void NormalCtrlState()
{
    if(RC_X.MotorSpeed >= 0)
    {
        RC_X.MotorSpeed *= RC_X.KnobLimit.L / 1000.0;
        MotorSpeed = __Map(RC_X.MotorSpeed, 0, 1000, MotorSpeed_MID, MotorSpeed_MAX);
    }
    else
    {
        if(RC_X.KnobLimit.L < 2)RC_X.MotorSpeed = 0;
        MotorSpeed = __Map(RC_X.MotorSpeed, -1000, 0, MotorSpeed_MIN, MotorSpeed_MID);
    }

    RC_X.SteerAngle *= RC_X.KnobLimit.R / 1000.0;

    if(-RC_X.SteerAngle >= 0) ServoAngle = map(-RC_X.SteerAngle, 0, 1000, ServoAngle_MID, ServoAngle_MAX);
    else ServoAngle = map(-RC_X.SteerAngle, -1000, 0, ServoAngle_MIN, ServoAngle_MID);
}

static void LostCtrlState()
{
    MotorSpeed = MotorSpeed_MID;
    ServoAngle = ServoAngle_MID;
}

void Task_Ctrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
        NormalCtrlState();
    else
        LostCtrlState();
    
    BrushlessMotor.write(MotorSpeed);
    SteerServo.write(ServoAngle);
}
