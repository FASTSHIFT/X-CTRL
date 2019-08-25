#include "FileGroup.h"

Servo Rudder_L;
Servo Rudder_R;
Servo BrushlessMotor;
float Rudder_L_Angle, Rudder_L_MID = (Rudder_L_MIN + Rudder_L_MAX) / 2;
float Rudder_R_Angle, Rudder_R_MID = (Rudder_R_MIN + Rudder_R_MAX) / 2;
uint16_t MotorSpeed, MotorSpeed_MID = 60;

void Task_MID_Setting()
{
    if(RC_Aircraft.Key & BT_L1)Rudder_L_MID--;
    else if(RC_Aircraft.Key & BT_L2)Rudder_L_MID++;
    LimitValue(Rudder_L_MID, Rudder_L_MIN, Rudder_L_MAX);

    if(RC_Aircraft.Key & BT_R1)Rudder_R_MID++;
    else if(RC_Aircraft.Key & BT_R2)Rudder_R_MID--;
    LimitValue(Rudder_R_MID, Rudder_R_MIN, Rudder_R_MAX);
	
	if(RC_Aircraft.Key & BT_OK) tone(Buzz_Pin,2000,1000);
}

void Task_Ctrl()
{
    float Limit_L = RC_Aircraft.KnobLimit.L / 1000.0;
    float Limit_R = RC_Aircraft.KnobLimit.R / 1000.0;
    float Angle_L, Angle_R;

    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        if(RC_Aircraft.Left.Y > 0)
        {
            MotorSpeed = map(RC_Aircraft.Left.Y * Limit_L, 0, CtrlOutput_MaxValue, MotorSpeed_MID, MotorSpeed_MAX);
        }
        else
        {
            MotorSpeed = MotorSpeed_MID;
        }
		
		Angle_L = RC_Aircraft.Right.Y * Limit_R - RC_Aircraft.Right.X * Limit_R;
        Angle_R = RC_Aircraft.Right.Y * Limit_R + RC_Aircraft.Right.X * Limit_R;
        LimitValue(Angle_L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        LimitValue(Angle_R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);

        if(Angle_L >= 0)
            Rudder_L_Angle = fmap(Angle_L, 0, CtrlOutput_MaxValue, Rudder_L_MID, Rudder_L_MAX);
        else
            Rudder_L_Angle = fmap(Angle_L, -CtrlOutput_MaxValue, 0, Rudder_L_MIN, Rudder_L_MID);

        if(-Angle_R >= 0)
            Rudder_R_Angle = fmap(-Angle_R, 0, CtrlOutput_MaxValue, Rudder_R_MID, Rudder_R_MAX);
        else
            Rudder_R_Angle = fmap(-Angle_R, -CtrlOutput_MaxValue, 0, Rudder_R_MIN, Rudder_R_MID);
    }
    else
    {
        MotorSpeed = MotorSpeed_MID;
        Rudder_L_Angle = Rudder_L_MID;
        Rudder_R_Angle = Rudder_R_MID;
    }
    BrushlessMotor.write(MotorSpeed);
    Rudder_L.write(Rudder_L_Angle);
    Rudder_R.write(Rudder_R_Angle);
}

#include "MusicPlay.h"
const MusicCode_t mc1[] = {
    1046, 80,
    1175, 80,
    1318, 80,
    0, 0
};

const MusicCode_t mc2[] = {
    100, 80,
    0, 80,
    100, 80,
    0, 0
};

void Task_BuzzRing()
{
    static MusicPlay mp1(Buzz_Pin, mc1, sizeof(mc1) / sizeof(MusicCode_t));
    static MusicPlay mp2(Buzz_Pin, mc2, sizeof(mc2) / sizeof(MusicCode_t));

    enum {Start = 1, Connect, Disonnect, Error};
    static uint8_t Sound_Now, Sound_Last = !Sound_Now;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Connect;
        if(Sound_Now != Sound_Last)
        {
            mp1.Play();
			tone(Buzz_Pin,1000,100);
            Sound_Last = Sound_Now;
        }
        mp1.Running(millis());
    }
    else if(!ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Disonnect;
        if(Sound_Now != Sound_Last)
        {
            mp2.Play();
			tone(Buzz_Pin,100,1000);
            Sound_Last = Sound_Now;
        }
        mp2.Running(millis());
    }
}
