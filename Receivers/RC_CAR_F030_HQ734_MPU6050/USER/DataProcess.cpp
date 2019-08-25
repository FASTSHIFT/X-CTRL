#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

Protocol_CarSS_t RC_CAR;
ConnectState_t ConnectState;

void NRF_CheckConnect(Protocol_CarSS_t* rc_car)
{
    static uint8_t DataPackage_Num_Last;
    if (rc_car->Info.DataPackage_Num != DataPackage_Num_Last)
    {
        ConnectState.TimePoint_Recv = millis();
        DataPackage_Num_Last = rc_car->Info.DataPackage_Num;
    }

    if(millis() < ConnectState.TimePoint_Recv + ConnectLost_TimeOut)
    {
        ConnectState.IsConnect = true;
    }
    else
    {
        ConnectState.IsConnect = false;
    }
}

void Task_NRF_Receive()
{
    static Protocol_CarSS_t RC_CAR_Temp;
    nrf.Recv((uint8_t*)&RC_CAR_Temp);

    NRF_CheckConnect(&RC_CAR_Temp);

    if(RC_CAR_Temp.Info.CtrlObject == CAR_ServoSteering)
    {
        ConnectState.IsCorrect = true;
        RC_CAR = RC_CAR_Temp;
    }
    else
    {
        ConnectState.IsCorrect = false;
    }
}

