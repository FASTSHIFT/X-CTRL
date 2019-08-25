#include "FileGroup.h"
#include "ComPrivate.h"
#include "Servo.h"

#define MotorSpeed_MAX 120
#define MotorSpeed_MID 60
#define MotorSpeed_MIN 20

Servo BrushlessMotor_L;
Servo BrushlessMotor_R;
float MotorSpeed = MotorSpeed_MID;
float Direction = 0;
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
        MotorSpeed = map(RC_X.Left.Y * L_limit, 0, CtrlOutput_MaxValue, MotorSpeed_MID, MotorSpeed_MAX);
    }
    else if(RC_X.Left.Y < -200)
    {
        MotorSpeed = map((RC_X.Left.Y + 200), -CtrlOutput_MaxValue, 0, MotorSpeed_MIN, MotorSpeed_MID);
    }
    else
    {
        MotorSpeed = MotorSpeed_MID;
    }

    if(IMUCtrl_EnablePit)
    {
        MotorSpeed = MotorSpeed_MID;
        Direction = constrain(RC_X.Left.X + RC_X.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else
    {
        Direction = RC_X.Right.X;
    }
	
	if(RC_X.Key & BT_UP)
	{
		digitalWrite(Light_Pin, HIGH);
	}
	
	if(RC_X.Key & BT_DOWN)
	{
		digitalWrite(Light_Pin, LOW);
	}
}

static void LostCtrl_Handler()
{
    MotorSpeed = MotorSpeed_MID;

    Direction = 0;
}

void Task_Ctrl()
{
    L_limit = __Map(RC_X.KnobLimit.L, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    R_limit = __Map(RC_X.KnobLimit.R, 0, CtrlOutput_MaxValue, 0.0, 1.0) * 0.8;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        NormalCtrl_Handler();
        IMUCtrl_Reset = !(L_limit > 0.01);
    }
    else
    {
        LostCtrl_Handler();
        IMUCtrl_Reset = true;
    }
    if(!IMUCtrl_Reset)
    {
        if(IMUCtrl_EnablePit)
        {
            IMUCtrl_EnablePit = !!(RC_X.Left.Y > 50);
        }
        else
        {
            IMUCtrl_EnablePit = !!(RC_X.Left.Y > 950);
        }
    }
    else
    {
        IMUCtrl_EnablePit = false;
    }

    IMUCtrl_Speed = MotorSpeed;
    IMUCtrl_Yaw = -__Map(Direction, -1000, 1000, -10.0, 10.0);

//    BrushlessMotor_L.write(MotorSpeed);
//    BrushlessMotor_R.write(MotorSpeed);
}
