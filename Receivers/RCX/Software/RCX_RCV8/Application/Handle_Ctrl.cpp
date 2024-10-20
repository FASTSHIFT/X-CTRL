#include "FileGroup.h"
#include "ComPrivate.h"
#include "Servo/Servo.h"

#define ServoAngle_MAX 120
#define ServoAngle_MIN 70
#define MotorSpeed_MAX 140
#define MotorSpeed_MIN 40

#define Limit_L (RCX::GetRxPackChannel(CH_LMT_L) / (float)RCX_CHANNEL_DATA_MAX)
#define Limit_R (RCX::GetRxPackChannel(CH_LMT_R) / (float)RCX_CHANNEL_DATA_MAX)

static Servo SteerServo;
static Servo BrushlessMotor;
static uint16_t ServoAngle, ServoAngle_MID = (ServoAngle_MIN + ServoAngle_MAX) / 2;
static uint16_t MotorSpeed, MotorSpeed_MID = 90;

void Init_MotorServo()
{
    BrushlessMotor.attach(Motor_Pin);
    BrushlessMotor.write(MotorSpeed_MID);
    SteerServo.attach(Servo_Pin);
    SteerServo.write(ServoAngle_MID);
}

static void Task_MID_Setting()
{
    uint8_t key = RCX::GetRxPackKey();
    if(key & BT_R2)ServoAngle_MID++;
    else if(key & BT_L2)ServoAngle_MID--;
    __LimitValue(ServoAngle_MID, ServoAngle_MIN, ServoAngle_MAX);
}

static void NormalCtrlState()
{
    int16_t motorVal = RCX::GetRxPackChannel(CH_JSL_Y);
    if(motorVal > 10)
    {
        MotorSpeed = __Map((motorVal * Limit_L), 0, RCX_CHANNEL_DATA_MAX, MotorSpeed_MID, MotorSpeed_MAX);
    }
    else
    {
        if(motorVal < -200)
            MotorSpeed = __Map((motorVal + 200), -RCX_CHANNEL_DATA_MAX, 0, MotorSpeed_MIN, MotorSpeed_MID);
        else
            MotorSpeed = MotorSpeed_MID;
    }

    int16_t steerVal = -RCX::GetRxPackChannel(CH_JSR_X);
    if(steerVal >= 0)
        ServoAngle = map(steerVal, 0, RCX_CHANNEL_DATA_MAX, ServoAngle_MID, ServoAngle_MAX);
    else 
        ServoAngle = map(steerVal, -RCX_CHANNEL_DATA_MAX, 0, ServoAngle_MIN, ServoAngle_MID);
    
//    Serial.printf("MotorSpeed:%d, ServoAngle:%d\r\n", MotorSpeed, ServoAngle);
}

static void LostCtrlState()
{
    MotorSpeed = MotorSpeed_MID;
    ServoAngle = ServoAngle_MID;
}

void Task_Ctrl(MTM::Task* task)
{
    __IntervalExecute(Task_MID_Setting(), 100);
    
    if(RCX::GetRxConnected())
        NormalCtrlState();
    else
        LostCtrlState();

    BrushlessMotor.write(MotorSpeed);
    SteerServo.write(ServoAngle);
}
