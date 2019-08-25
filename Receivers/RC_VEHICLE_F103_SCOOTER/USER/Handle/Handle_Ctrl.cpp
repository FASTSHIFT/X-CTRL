#include "FileGroup.h"
#include "ComPrivate.h"
#include "SpeedCtrl.h"
#include "Servo.h"

static Servo BrushlessMotor;
float MotorSpeed = MotorSpeed_MID, MotorSpeed_target = MotorSpeed_MID;
float L_limit, R_limit;

void Init_Motor()
{
    BrushlessMotor.attach(Motor_Pin);
    BrushlessMotor.write(MotorSpeed_MID);
}

static void NormalCtrl_Handler()
{
    if(RC_X.Left.Y > 80)
    {
        IsSpeedUp = true;
        SpeedUp_Handler();   
    }
    else if(RC_X.Left.Y < -200)
    {
        IsSpeedUp = false;
        Brake_Handler();
    }
    else
    {
        IsSpeedUp = true;
        EnableSlider = true;
        MotorSpeed_target = MotorSpeed_MID;  
    }
}

static void LostCtrl_Handler()
{
    mtm.TaskList[TP_SpeedGenerator].IntervalTime = 10;
    MotorSpeed_target = MotorSpeed_MID;
}

void Task_Ctrl()
{
    L_limit = __Map(RC_X.KnobLimit.L, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    R_limit = __Map(RC_X.KnobLimit.R, 0, CtrlOutput_MaxValue, 0.0, 1.0) * 0.8;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
        NormalCtrl_Handler();
    else
        LostCtrl_Handler();
    
    BrushlessMotor.write(MotorSpeed);
}
