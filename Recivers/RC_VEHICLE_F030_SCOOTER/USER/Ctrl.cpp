#include "FileGroup.h"

Servo BrushlessMotor;
float MotorSpeed = MotorSpeed_MID, MotorSpeed_target = MotorSpeed_MID;
float L_limit, R_limit;

void Task_SpeedGenerator()
{
    if(MotorSpeed < MotorSpeed_target)
    {
        if(MotorSpeed > (MotorSpeed_MID + (MotorSpeed_target - MotorSpeed_MID) * 0.5))
            MotorSpeed += 0.5;
        else
            MotorSpeed+=1;
    }
    else if(MotorSpeed > MotorSpeed_target)
        MotorSpeed--;
}

void Task_Ctrl()
{
    L_limit = fmap(RC_X.KnobLimit.L, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    R_limit = fmap(RC_X.KnobLimit.R, 0, CtrlOutput_MaxValue, 0.0, 1.0) * 0.8;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        if(RC_X.Left.Y > 5)
        {
            MotorSpeed_target = map(RC_X.Left.Y * L_limit, 0, CtrlOutput_MaxValue, MotorSpeed_MID, MotorSpeed_MAX);
            mtm.TaskList[3].IntervalTime = 100 * (1.0 - R_limit);
        }
        else
        {
            if(RC_X.Left.Y < -200)
                mtm.TaskList[3].IntervalTime = 30;
            else if(RC_X.Left.Y < -500)
                mtm.TaskList[3].IntervalTime = 20;
            else if(RC_X.Left.Y < -900)
                mtm.TaskList[3].IntervalTime = 10;
            MotorSpeed_target = MotorSpeed_MID;
        }
    }
    else
    {
        mtm.TaskList[3].IntervalTime = 80;
        MotorSpeed_target = MotorSpeed_MID;
    }
    BrushlessMotor.write(MotorSpeed);
}
