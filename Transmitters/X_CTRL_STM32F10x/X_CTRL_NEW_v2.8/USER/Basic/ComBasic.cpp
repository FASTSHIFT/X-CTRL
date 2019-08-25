#include "FileGroup.h"
#include "ComPrivate.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_TxBuff[32];
uint8_t NRF_RxBuff[32];
uint8_t NRF_AddressConfig[NRF_AddressNum * 5]={
	45,23,78,19,61, //ADDR:0
	75,75,75,75,75, //ADDR:1
	12,34,56,78,90, //ADDR:2
	12,90,17,44,55, //ADDR:3
	10,93,70,72,31  //ADDR:4
};
uint8_t NRF_SignalStrength = 0;
NRF_Config_TypeDef NRF_Cfg = {0, 0, 40};

bool RF_State = OFF;
bool PassBack_State = OFF;
bool Handshake_State = ON;

void Init_NRF()
{
    nrf.SetAddress(NRF_AddressConfig + NRF_Cfg.Address * 5);
    nrf.SetFreqency(NRF_Cfg.Freq);
    if(NRF_Cfg.Speed == 0)
        nrf.SetSpeed(nrf.SPEED_250Kbps);
    else if(NRF_Cfg.Speed == 1)
        nrf.SetSpeed(nrf.SPEED_1Mbps);
    else if(NRF_Cfg.Speed == 2)
        nrf.SetSpeed(nrf.SPEED_2Mbps);
    nrf.init(nrf.TXRX_MODE, NRF_TxBuff ,sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
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
        LimitValue(CarDS.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        LimitValue(CarDS.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else if(CTRL.Info.CtrlObject == CAR_ServoSteering)
    {
        CarSS.Info = CTRL.Info;
        CarSS.Key = CTRL.Key;
        CarSS.KnobLimit = CTRL.KnobLimit;
        CarSS.KnobCab = CTRL.KnobCab;

        CarSS.MotorSpeed = CTRL.Left.Y;
        CarSS.SteerAngle = CTRL.Right.X;

        LimitValue(CarSS.MotorSpeed, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        LimitValue(CarSS.SteerAngle, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

void Task_TransferData()
{
    if(RF_State == OFF) return;
    
    LoadDataPack();
    CTRL.Info.DataPackage_Num++;
    switch(CTRL.Info.CtrlObject)
    {
    case X_COMMON:
        memcpy(NRF_TxBuff, &COMMON_X, sizeof(COMMON_X));
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
        extern bool Is_CommonPassbackReading;
        if(PassBack_State)
        {
            nrf.TranRecv(NRF_TxBuff, NRF_RxBuff);
            if(CTRL.Info.CtrlObject == CAR_ServoSteering)
                memcpy(&CarSS_Passback, NRF_RxBuff, sizeof(CarSS_Passback));
            else if(CTRL.Info.CtrlObject == X_COMMON && !Is_CommonPassbackReading)
                memcpy(&Common_Passback, NRF_RxBuff, sizeof(Common_Passback));
        }
        else
        {
            nrf.Tran(NRF_TxBuff);
        }
    }
    else if(ConnectState.Pattern == Pattern_USART)
    {
        BlueTooth.write(NRF_TxBuff, sizeof(NRF_TxBuff));
    }
}
