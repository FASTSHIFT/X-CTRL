#include "FileGroup.h"
#include "ComPrivate.h"
#include "Servo.h"

#define MotorSpeed_MAX 130
#define MotorSpeed_MID 100
#define MotorSpeed_MIN 62 

#define ServoAngle_MAX 116
#define ServoAngle_MIN 54

#define Limit_L (RC_X.KnobLimit.L / 1000.0)
#define Limit_R (RC_X.KnobLimit.R / 1000.0)

static Servo ThrServo;
static Servo SteerServo;
uint16_t ServoAngle_MID = (ServoAngle_MIN + ServoAngle_MAX) / 2;
uint16_t ServoAngle = ServoAngle_MID; 
uint16_t MotorSpeed = MotorSpeed_MID;

void Init_Servo()
{
    ThrServo.attach(CH4_Pin);
    ThrServo.write(MotorSpeed);

    SteerServo.attach(CH3_Pin);
    SteerServo.write(ServoAngle);
}

void Task_MID_Setting()
{
    if(RC_X.Key & BT_L2)ServoAngle_MID++;
    else if(RC_X.Key & BT_R2)ServoAngle_MID--;
    __LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);
}

static void NormalCtrlState()
{
    if(RC_X.MotorSpeed > 10)
    {
        MotorSpeed = __Map((RC_X.MotorSpeed * Limit_L), 0, 1000, MotorSpeed_MID, MotorSpeed_MAX);
    }
    else
    {
        if(RC_X.MotorSpeed < -10)
            MotorSpeed = __Map((RC_X.MotorSpeed * Limit_L), -1000, 0, MotorSpeed_MIN, MotorSpeed_MID);
        else
            MotorSpeed = MotorSpeed_MID;
    }

    if(-RC_X.SteerAngle >= 0) 
        ServoAngle = __Map((-RC_X.SteerAngle * Limit_R), 0, 1000, ServoAngle_MID, ServoAngle_MAX);
    else 
        ServoAngle = __Map((-RC_X.SteerAngle * Limit_R), -1000, 0, ServoAngle_MIN, ServoAngle_MID);
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
    
    ThrServo.write(MotorSpeed);
    SteerServo.write(ServoAngle);
}
