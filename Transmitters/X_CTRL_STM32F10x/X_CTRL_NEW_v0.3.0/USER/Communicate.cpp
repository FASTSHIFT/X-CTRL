#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

Protocol_CarDS_t CarDS;
Protocol_CarSS_t CarSS;
Protocol_Common_t CTRL;
ConnectState_t ConnectState;
float Limit_L, Limit_R;

void Task_LoadCtrlMode()
{
    Limit_L = CTRL.KnobLimit.L / (float)CtrlOutput_MaxValue;
    Limit_R = CTRL.KnobLimit.R / (float)CtrlOutput_MaxValue;

    if(CTRL.Info.CtrlObject == X_COMMON)
    {

    }
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
    {
        CarDS.Info = CTRL.Info;
        CarDS.Key = CTRL.Key;
        CarDS.KnobLimit = CTRL.KnobLimit;
        CarDS.KnobCab = CTRL.KnobCab;

        switch(CTRL.Info.CtrlMode)
        {
        case CtrlMode_Tank:
            CarDS.MotorSpeed.L = JS_L.Y * Limit_L;
            CarDS.MotorSpeed.R = JS_R.Y * Limit_R;
            break;

        case CtrlMode_Race:
            if (JS_L.Y >= 0)
            {
                CarDS.MotorSpeed.L = JS_L.Y * Limit_L + JS_R.X * Limit_R;
                CarDS.MotorSpeed.R = JS_L.Y * Limit_L - JS_R.X * Limit_R;
            }
            else
            {
                CarDS.MotorSpeed.L = JS_L.Y * Limit_L - JS_R.X * Limit_R;
                CarDS.MotorSpeed.R = JS_L.Y * Limit_L + JS_R.X * Limit_R;
            }
            break;

        case CtrlMode_Expert:
            if (JS_L.Y >= 0)
            {
                CarDS.MotorSpeed.L = JS_L.Y * Limit_L + JS_L.X * Limit_L * 0.5;
                CarDS.MotorSpeed.R = JS_L.Y * Limit_L - JS_L.X * Limit_L * 0.5;
            }
            else
            {
                CarDS.MotorSpeed.L = JS_L.Y * Limit_L - JS_L.X * Limit_L * 0.5;
                CarDS.MotorSpeed.R = JS_L.Y * Limit_L + JS_L.X * Limit_L * 0.5;
            }
            break;
        }
        LimitValue(CarDS.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        LimitValue(CarDS.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else if(CTRL.Info.CtrlObject == CAR_ServoSteering)
    {
        CarSS.Info = CTRL.Info;
        CarSS.Key = CTRL.Key;
        CarSS.KnobLimit = CTRL.KnobLimit;
        CarSS.KnobCab = CTRL.KnobCab;

        CarSS.MotorSpeed = JS_L.Y;
        CarSS.SteerAngle = JS_R.X;
		//CarSS.SteerAngle = MPU_Data.X;

        LimitValue(CarSS.MotorSpeed, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        LimitValue(CarSS.SteerAngle, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

void Task_TransferData()
{
    CTRL.Info.DataPackage_Num++;
    switch(CTRL.Info.CtrlObject)
    {
    case X_COMMON:
		CTRL.Right.X = MPU_Data.X;
        if(ConnectState.Pattern == Pattern_NRF)
            nrf.Tran((uint8_t*)&CTRL);
        else
            BlueTooth.write((uint8_t*)&CTRL, sizeof(CTRL));
        break;

    case CAR_DifferentalSteering:
        if(ConnectState.Pattern == Pattern_NRF)
            nrf.Tran((uint8_t*)&CarDS);
        else
            BlueTooth.write((uint8_t*)&CarDS, sizeof(CarDS));
        break;

    case CAR_ServoSteering:
        if(ConnectState.Pattern == Pattern_NRF)
            nrf.Tran((uint8_t*)&CarSS);
        else
            BlueTooth.write((uint8_t*)&CarSS, sizeof(CarSS));
        break;
    }
}
