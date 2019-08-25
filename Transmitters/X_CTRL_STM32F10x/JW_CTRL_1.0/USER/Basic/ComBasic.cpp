#include "FileGroup.h"
#include "ComPrivate.h"

#define ConnectLost_TimeOut 500

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_TxBuff[32];
uint8_t NRF_RxBuff[32];
int16_t NRF_SignalStrength = 0;

bool State_RF = OFF;
bool State_PassBack = OFF;
bool State_Handshake = ON;

bool Init_NRF()
{
    nrf.init(nrf.TXRX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    return nrf.IsDetect();
}

Protocol_CarDS_t CarDS;
Protocol_CarSS_t CarSS;
Protocol_Common_t CTRL;
Protocol_Common_t COMMON_X;
Protocol_Passback_8xChannel_t CarSS_Passback;
Protocol_Passback_CommonDisplay_t Common_Passback;
ConnectState_t ConnectState;

void LoadDataPack()
{
    float Limit_L = CTRL.KnobLimit.L / (float)CtrlOutput_MaxValue;
    float Limit_R = CTRL.KnobLimit.R / (float)CtrlOutput_MaxValue;

    if(CTRL.Info.CtrlObject == X_COMMON)
    {
        COMMON_X = CTRL;
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
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L;
            CarDS.MotorSpeed.R = CTRL.Right.Y * Limit_R;
            break;

        case CtrlMode_Race:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L + CTRL.Right.X * Limit_R;
            CarDS.MotorSpeed.R = CTRL.Left.Y * Limit_L - CTRL.Right.X * Limit_R;
            break;

        case CtrlMode_Expert:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L + CTRL.Left.X * Limit_L * 0.5;
            CarDS.MotorSpeed.R = CTRL.Left.Y * Limit_L - CTRL.Left.X * Limit_L * 0.5;
            break;
        }
        __LimitValue(CarDS.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarDS.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else if(CTRL.Info.CtrlObject == CAR_ServoSteering)
    {
        CarSS.Info = CTRL.Info;
        CarSS.Key = CTRL.Key;
        CarSS.KnobLimit = CTRL.KnobLimit;
        CarSS.KnobCab = CTRL.KnobCab;

        CarSS.MotorSpeed = CTRL.Left.Y;
        CarSS.SteerAngle = CTRL.Left.X;

        __LimitValue(CarSS.MotorSpeed, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarSS.SteerAngle, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

void Task_TransferData()
{
    if(State_RF == OFF) return;

    LoadDataPack();
    CTRL.Info.DataPackage_Num++;
    switch(CTRL.Info.CtrlObject)
    {
    case X_COMMON:
        *((Protocol_Common_t*)NRF_TxBuff) = COMMON_X;
        break;

    case CAR_DifferentalSteering:
        *((Protocol_CarDS_t*)NRF_TxBuff) = CarDS;
        break;

    case CAR_ServoSteering:
        *((Protocol_CarSS_t*)NRF_TxBuff) = CarSS;
        break;
    }

    if(ConnectState.Pattern == Pattern_NRF)
    {
        if(State_PassBack)
        {
            nrf.TranRecvSafe(NRF_TxBuff, NRF_RxBuff);
//            if(CTRL.Info.CtrlObject == CAR_ServoSteering)
//                CarSS_Passback = *((Protocol_Passback_8xChannel_t*)NRF_RxBuff);
            if(!Is_CommonPassbackReading)
                Common_Passback = *((Protocol_Passback_CommonDisplay_t*)NRF_RxBuff);
        }
        else
            nrf.Tran(NRF_TxBuff);
    }
}
