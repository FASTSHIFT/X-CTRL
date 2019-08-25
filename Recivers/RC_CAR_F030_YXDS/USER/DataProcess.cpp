#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
Protocol_CarDS_t DS_CAR;
ConnectState_t ConnectState;

void NRF_CheckConnect(Protocol_CarDS_t* ds_car)
{
    static uint8_t DataPackage_Num_Last;
    if (ds_car->Info.DataPackage_Num != DataPackage_Num_Last)
    {
        ConnectState.TimePoint_Recv = millis();
        DataPackage_Num_Last = ds_car->Info.DataPackage_Num;
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
    static Protocol_CarDS_t DS_CAR_Temp;
    nrf.Recv(NRF_RxBuff);
	memcpy(&DS_CAR_Temp, NRF_RxBuff, sizeof(DS_CAR_Temp));

    NRF_CheckConnect(&DS_CAR_Temp);

    if(DS_CAR_Temp.Info.CtrlObject == CAR_DifferentalSteering)
    {
        ConnectState.IsCorrect = true;
        DS_CAR = DS_CAR_Temp;
    }
    else
    {
        ConnectState.IsCorrect = false;
    }
}

