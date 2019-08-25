#include "FileGroup.h"

Servo ServoDir;
int MotorSpeed_1 = MotorSpeed_MID, MotorSpeed_2 = MotorSpeed_MID;
int MotorSpeed_Compensator = 0;
int Servo_MID = (Servo_MAX + Servo_MIN) / 2;
int ServoPos = Servo_MID;

void Task_MID_Setting()
{
    if(RC_Aircraft.Key & BT_L1)MotorSpeed_Compensator ++;
    else if(RC_Aircraft.Key & BT_L2)MotorSpeed_Compensator --;
    LimitValue(MotorSpeed_Compensator, -500, 500);

    if(RC_Aircraft.Key & BT_R1)Servo_MID++;
    else if(RC_Aircraft.Key & BT_R2)Servo_MID--;
    LimitValue(Servo_MID, Servo_MIN, Servo_MAX);

    if(RC_Aircraft.Key & BT_OK) tone(Buzz_Pin, 2000, 1000);
}

void Task_Ctrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        float Limit_L = RC_Aircraft.KnobLimit.L / 1000.0;
        float Limit_R = RC_Aircraft.KnobLimit.R / 1000.0;

        if(RC_Aircraft.Left.Y > 5)
        {
            int synt = map(RC_Aircraft.Left.Y * Limit_L, 0, CtrlOutput_MaxValue, MotorSpeed_MID, 950);
            int diff = map(RC_Aircraft.Right.X * Limit_R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -500, 500);
            MotorSpeed_1 = synt + diff + MotorSpeed_Compensator;
            MotorSpeed_2 = synt - diff - MotorSpeed_Compensator;
            LimitValue(MotorSpeed_1, MotorSpeed_MID, 980);
            LimitValue(MotorSpeed_2, MotorSpeed_MID, 980);
        }
        else
        {
            MotorSpeed_1 = MotorSpeed_2 = MotorSpeed_MID;
        }

        if(RC_Aircraft.Right.Y >= 0)
        {
            ServoPos = map(RC_Aircraft.Right.Y, 0, CtrlOutput_MaxValue, Servo_MID, Servo_MAX);
        }
        else
        {
            ServoPos = map(RC_Aircraft.Right.Y, -CtrlOutput_MaxValue, 0, Servo_MIN, Servo_MID);
        }
    }
    else
    {
        MotorSpeed_1 = MotorSpeed_2 = MotorSpeed_MID;
        ServoPos = Servo_MID;
    }
    analogWrite(Motor1_Pin, MotorSpeed_1);
    analogWrite(Motor2_Pin, MotorSpeed_2);
    ServoDir.write(ServoPos);
}

//#include "MusicPlay.h"
//const MusicCode_t mc1[] = {
//    1046, 80,
//    1175, 80,
//    1318, 80,
//    0, 0
//};

//const MusicCode_t mc2[] = {
//    100, 80,
//    0, 80,
//    100, 80,
//    0, 0
//};

void Task_BuzzRing()
{
//    static MusicPlay mp1(Buzz_Pin, mc1, sizeof(mc1) / sizeof(MusicCode_t));
//    static MusicPlay mp2(Buzz_Pin, mc2, sizeof(mc2) / sizeof(MusicCode_t));

    enum {Start = 1, Connect, Disonnect, Error};
    static uint8_t Sound_Now, Sound_Last = !Sound_Now;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Connect;
        if(Sound_Now != Sound_Last)
        {
//            mp1.Play();
            tone(Buzz_Pin, 1000, 100);
            Sound_Last = Sound_Now;
        }
//        mp1.Running(millis());
    }
    else if(!ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Disonnect;
        if(Sound_Now != Sound_Last)
        {
//            mp2.Play();
            tone(Buzz_Pin, 100, 1000);
            Sound_Last = Sound_Now;
        }
//        mp2.Running(millis());
    }
}
