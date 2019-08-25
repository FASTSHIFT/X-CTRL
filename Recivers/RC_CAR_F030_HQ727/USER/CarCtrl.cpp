#include "FileGroup.h"

Servo SteerServo;
Servo BrushlessMotor;
uint16_t ServoAngle, ServoAngle_MID = (ServoAngle_MIN + ServoAngle_MAX) / 2;
uint16_t MotorSpeed, MotorSpeed_MID = 60;
uint32_t MotorTimes;
uint32_t tmpMotorTimes;
uint32_t LastMotorInterrupt;

void Task_MID_Setting()
{
    //MotorSpeed_MID = map(RC_CAR.KnobCab.R, -1000, 1000, MotorSpeed_MIN, MotorSpeed_MAX);

    if(RC_CAR.Key & BT_R2)ServoAngle_MID++;
    else if(RC_CAR.Key & BT_L2)ServoAngle_MID--;
    LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);

    static int8_t LED_pwm;
    if(RC_CAR.Key & BT_UP)LED_pwm += 10;
    else if(RC_CAR.Key & BT_DOWN)LED_pwm -= 10;
    LimitValue(LED_pwm, 0, 100);
    analogWrite(LED_Pin, LED_pwm);
	
	if(RC_CAR.Key & BT_RIGHT)tone(Buzz_Pin,1000,100);
	else if(RC_CAR.Key & BT_LEFT)tone(Buzz_Pin,100,100);

//    if(RC_CAR.Key & BT_L1)ServoAngle_Limit-=0.05;
//    else if(RC_CAR.Key & BT_R1)ServoAngle_Limit+=0.05;
//	LimitValue(ServoAngle_Limit, 0.0, 1.0);
}

void Task_CarCtrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        if(RC_CAR.MotorSpeed >= 0) 
		{
			RC_CAR.MotorSpeed *= RC_CAR.KnobLimit.L / 1000.0;
			MotorSpeed = map(RC_CAR.MotorSpeed, 0, 1000, MotorSpeed_MID, MotorSpeed_MAX);
		}
        else 
		{
			if(RC_CAR.KnobLimit.L < 2)RC_CAR.MotorSpeed = 0;
			MotorSpeed = map(RC_CAR.MotorSpeed, -1000, 0, MotorSpeed_MIN, MotorSpeed_MID);
		}

        RC_CAR.SteerAngle *= RC_CAR.KnobLimit.R / 1000.0;

        if(RC_CAR.SteerAngle >= 0) ServoAngle = map(RC_CAR.SteerAngle, 0, 1000, ServoAngle_MID, ServoAngle_MAX);
        else ServoAngle = map(RC_CAR.SteerAngle, -1000, 0, ServoAngle_MIN, ServoAngle_MID);

    }
    else
    {
        MotorSpeed = MotorSpeed_MID;
        ServoAngle = ServoAngle_MID;
    }
    BrushlessMotor.write(MotorSpeed);
    SteerServo.write(ServoAngle);
	
	if(millis() - LastMotorInterrupt > 200)
	{
		MotorTimes = tmpMotorTimes;
		tmpMotorTimes = 0;
		LastMotorInterrupt = millis();
	}
	CarSS_Slave.Channel_1 = MotorTimes / 1.14 /2.0;
}

#include "MusicPlay.h"

const MusicCode_t mc1[]={
	1046, 80,
	1175, 80,
	1318, 80,
	0,0
};

const MusicCode_t mc2[]={
	100, 80,
	0, 80,
	100, 80,
	0,0
};

void Task_BuzzRing()
{
	static MusicPlay mp1(Buzz_Pin,mc1,sizeof(mc1)/sizeof(MusicCode_t));
	static MusicPlay mp2(Buzz_Pin,mc2,sizeof(mc2)/sizeof(MusicCode_t));
	
	enum {Start = 1,Connect, Disonnect, Error};
	static uint8_t Sound_Now,Sound_Last=!Sound_Now;
	if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Connect;
        if(Sound_Now != Sound_Last)
        {
            mp1.Play();
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
			Sound_Last = Sound_Now;
        }
		mp2.Running(millis());
    }
}

void SpeedCalc_Handler()
{
	tmpMotorTimes ++;
}
