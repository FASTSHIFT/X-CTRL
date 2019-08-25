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
uint8_t NRF_AddressConfig[NRF_AddressNum * 5] = {
    45, 23, 78, 19, 61, //ADDR:0
    75, 75, 75, 75, 75, //ADDR:1
    12, 34, 56, 78, 90, //ADDR:2
    12, 90, 17, 44, 55, //ADDR:3
    10, 93, 70, 72, 31, //ADDR:4
    0//ADDR: Handshake
};
int16_t NRF_SignalStrength = 0;
NRF_Config_TypeDef NRF_Cfg = {5, 0, 40};//@ADDR Handshake, Speed 250Kbps, 2440Mhz

bool State_RF = OFF;
bool State_PassBack = ON;
bool State_Handshake = ON;
bool Lock_CommonPassback = false;

bool Init_NRF()
{
    nrf.SetAddress(NRF_AddressConfig + NRF_Cfg.Address * 5);
    nrf.SetFreqency(NRF_Cfg.Freq);
    if(NRF_Cfg.Speed == 0)
        nrf.SetSpeed(nrf.SPEED_250Kbps);
    else if(NRF_Cfg.Speed == 1)
        nrf.SetSpeed(nrf.SPEED_1Mbps);
    else if(NRF_Cfg.Speed == 2)
        nrf.SetSpeed(nrf.SPEED_2Mbps);
    nrf.init(nrf.TXRX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    return nrf.IsDetect();
}

Protocol_CarDS_t CarDS;
Protocol_CarSS_t CarSS;
Protocol_CommonPlus_t CTRL;
Protocol_Common_t COMMON_X;
Protocol_Passback_CommonDisplay_t Common_Passback;
ConnectState_t ConnectState;

static void LoadDataPack()
{
    float Limit_L = CTRL.Common.KnobLimit.L / (float)CtrlOutput_MaxValue;
    float Limit_R = CTRL.Common.KnobLimit.R / (float)CtrlOutput_MaxValue;

    if(CTRL.Common.Info.CtrlObject == X_COMMON)
    {
        COMMON_X = CTRL.Common;
        COMMON_X.ReferenceClock = millis();
    }
    else if(CTRL.Common.Info.CtrlObject == CAR_DifferentalSteering)
    {
        CarDS.Info = CTRL.Common.Info;
        CarDS.Key = CTRL.Common.Key;
        CarDS.KnobLimit = CTRL.Common.KnobLimit;
        CarDS.KnobCab = CTRL.Common.KnobCab;
        CarDS.ReferenceClock = millis();

        switch(CTRL.Common.Info.CtrlMode)
        {
        case CtrlMode_Tank:
            CarDS.MotorSpeed.L = CTRL.Common.Left.Y * Limit_L;
            CarDS.MotorSpeed.R = CTRL.Common.Right.Y * Limit_R;
            break;

        case CtrlMode_Race:
            CarDS.MotorSpeed.L = CTRL.Common.Left.Y * Limit_L + CTRL.Common.Right.X * Limit_R;
            CarDS.MotorSpeed.R = CTRL.Common.Left.Y * Limit_L - CTRL.Common.Right.X * Limit_R;
            break;

        case CtrlMode_Expert:
            CarDS.MotorSpeed.L = CTRL.Common.Left.Y * Limit_L + CTRL.Common.Left.X * Limit_L * 0.5;
            CarDS.MotorSpeed.R = CTRL.Common.Left.Y * Limit_L - CTRL.Common.Left.X * Limit_L * 0.5;
            break;
        }
        __LimitValue(CarDS.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarDS.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else if(CTRL.Common.Info.CtrlObject == CAR_ServoSteering)
    {
        CarSS.Info = CTRL.Common.Info;
        CarSS.Key = CTRL.Common.Key;
        CarSS.KnobLimit = CTRL.Common.KnobLimit;
        CarSS.KnobCab = CTRL.Common.KnobCab;

        CarSS.MotorSpeed = CTRL.Common.Left.Y;
        CarSS.SteerAngle = CTRL.Common.Right.X;
        CarSS.ReferenceClock = millis();

        __LimitValue(CarSS.MotorSpeed, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarSS.SteerAngle, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

void Task_TransferData()
{
    if(State_RF == OFF) return;

    LoadDataPack();
    CTRL.Common.Info.DataPackage_Num++;
    switch(CTRL.Common.Info.CtrlObject)
    {
    case X_COMMON:
        __TypeExplain(Protocol_Common_t, NRF_TxBuff) = COMMON_X;
        break;

    case CAR_DifferentalSteering:
        __TypeExplain(Protocol_CarDS_t, NRF_TxBuff) = CarDS;
        break;

    case CAR_ServoSteering:
        __TypeExplain(Protocol_CarSS_t, NRF_TxBuff) = CarSS;
        break;
    }

    if(ConnectState.Pattern == Pattern_NRF)
    {   
        if(State_PassBack)
        {
            nrf.TranRecvSafe(NRF_TxBuff, NRF_RxBuff);
            if(!Lock_CommonPassback)
                Common_Passback = __TypeExplain(Protocol_Passback_CommonDisplay_t, NRF_RxBuff);
        }
        else
            nrf.Tran(NRF_TxBuff);
    }
}
