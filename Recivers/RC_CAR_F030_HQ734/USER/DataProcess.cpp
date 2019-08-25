#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];
Protocol_CarSS_t RC_CAR;
ConnectState_t ConnectState;
Slave_DataPack_t CarSS_Slave;

bool PassBack_State = ON;

 void NRF_Init()
{
    if(PassBack_State)
        nrf.init(RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    else
        nrf.init(RX_MODE, sizeof(NRF_RxBuff));
    while(!nrf.IsDetect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
    }
}

bool NRF_CheckConnect(Protocol_CarSS_t* rc_car)
{
    static uint8_t DataPackage_Num_Last;
    if (rc_car->Info.DataPackage_Num != DataPackage_Num_Last)
    {
        ConnectState.TimePoint_Recv = millis();
        DataPackage_Num_Last = rc_car->Info.DataPackage_Num;
    }

    if(millis() < ConnectState.TimePoint_Recv + ConnectLost_TimeOut)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Task_NRF_Receive()
{
    static Protocol_CarSS_t RC_CAR_Temp;
    if(PassBack_State)
    {
        memcpy(NRF_TxBuff, &CarSS_Slave, sizeof(CarSS_Slave));
        nrf.RecvTran(NRF_RxBuff, NRF_TxBuff);
    }
    else
    {
        nrf.Recv(NRF_RxBuff);
    }
    memcpy(&RC_CAR_Temp, NRF_RxBuff, sizeof(RC_CAR_Temp));

    ConnectState.IsConnect = NRF_CheckConnect(&RC_CAR_Temp);

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
