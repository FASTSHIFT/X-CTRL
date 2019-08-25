#include "FileGroup.h"
#include "ComPrivate.h"
#include "Servo.h"

#define ServoAngle_MAX 148
#define ServoAngle_MIN 107
#define MotorSpeed_MAX 120
#define MotorSpeed_MIN 0

#define Limit_L (RC_X.KnobLimit.L / 1000.0)
#define Limit_R (RC_X.KnobLimit.R / 1000.0)

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
    if(RC_X.Key & BT_R2)ServoAngle_MID++;
    else if(RC_X.Key & BT_L2)ServoAngle_MID--;
    __LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);

    static int8_t LED_pwm;
    if(RC_X.Key & BT_UP)LED_pwm += 10;
    else if(RC_X.Key & BT_DOWN)LED_pwm -= 10;
    __LimitValue(LED_pwm, 0, 100);
    analogWrite(FrontLED_Pin, LED_pwm);
}

static void NormalCtrlState()
{
    if(RC_X.MotorSpeed > 10)
    {
        MotorSpeed = __Map((RC_X.MotorSpeed * Limit_L), 0, 1000, MotorSpeed_MID, MotorSpeed_MAX);
    }
    else
    {
        if(RC_X.MotorSpeed < -200)
            MotorSpeed = __Map((RC_X.MotorSpeed + 200), -1000, 0, MotorSpeed_MIN, MotorSpeed_MID);
        else
           MotorSpeed = MotorSpeed_MID;
    }

    if(RC_X.SteerAngle >= 0)
        ServoAngle = map(RC_X.SteerAngle, 0, 1000, ServoAngle_MID, ServoAngle_MAX);
    else 
        ServoAngle = map(RC_X.SteerAngle, -1000, 0, ServoAngle_MIN, ServoAngle_MID);
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
