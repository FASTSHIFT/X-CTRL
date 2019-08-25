#include "FileGroup.h"
#include "ComPrivate.h"
#include "Servo.h"

#define MotorSpeed_MAX 120
#define MotorSpeed_MID 60
#define MotorSpeed_MIN 20

Servo BrushlessMotor_L;
Servo BrushlessMotor_R;
float MotorSpeed = MotorSpeed_MID;
float L_limit, R_limit;

void Init_Motor()
{
    BrushlessMotor_L.attach(Motor_L_Pin);
    BrushlessMotor_L.write(MotorSpeed_MID);
    BrushlessMotor_R.attach(Motor_R_Pin);
    BrushlessMotor_R.write(MotorSpeed_MID);
}

static void NormalCtrl_Handler()
{
    if(RC_X.Left.Y > 80)
    {
        MotorSpeed = __Map(RC_X.Left.Y * L_limit, 0, CtrlOutput_MaxValue, MotorSpeed_MID, MotorSpeed_MAX);
    }
    else if(RC_X.Left.Y < -200)
    {
        MotorSpeed = __Map((RC_X.Left.Y + 200), -CtrlOutput_MaxValue, 0, MotorSpeed_MIN, MotorSpeed_MID);
    }
    else
    {
        MotorSpeed = MotorSpeed_MID;
    }
}

static void LostCtrl_Handler()
{
    MotorSpeed = MotorSpeed_MID;
}

void Task_Ctrl()
{
    L_limit = __Map(RC_X.KnobLimit.L, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    R_limit = __Map(RC_X.KnobLimit.R, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        NormalCtrl_Handler();
    }
    else
    {
        LostCtrl_Handler();
    }

    BrushlessMotor_L.write(MotorSpeed);
    BrushlessMotor_R.write(MotorSpeed);
}
