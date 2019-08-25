#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_TxBuff[32];
uint8_t NRF_RxBuff[32];
bool RF_State = ON;
bool PassBack_State = ON;

void Init_NRF()
{
    nrf.SetAddress(NRF_AddressConfig);
    nrf.init(TXRX_MODE, (uint8_t*)&NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
}

Protocol_CarDS_t CarDS;
Protocol_CarSS_t CarSS;
Protocol_Common_t CTRL;
Slave_DataPack_t CarSS_Slave;
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
	if(RF_State == OFF) return;
	
    CTRL.Info.DataPackage_Num++;
    switch(CTRL.Info.CtrlObject)
    {
    case X_COMMON:
        CTRL.Right.X = MPU_Data.X;
		//CTRL.Right.Y = MPU_Data.Y;
        memcpy(NRF_TxBuff, &CTRL, sizeof(CTRL));
        break;

    case CAR_DifferentalSteering:
        memcpy(NRF_TxBuff, &CarDS, sizeof(CarDS));
        break;

    case CAR_ServoSteering:
        memcpy(NRF_TxBuff, &CarSS, sizeof(CarSS));
        break;
    }

    if(ConnectState.Pattern == Pattern_NRF)
	{
        if(PassBack_State)
		{			
			nrf.TranRecv(NRF_TxBuff, NRF_RxBuff);
            memcpy(&CarSS_Slave, NRF_RxBuff, sizeof(CarSS_Slave));
		}
		else
		{
			nrf.Tran(NRF_TxBuff);
		}
	}
    else
        BlueTooth.write(NRF_TxBuff, sizeof(NRF_TxBuff));
}
