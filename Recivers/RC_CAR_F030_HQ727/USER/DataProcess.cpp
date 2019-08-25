#include "FileGroup.h"
#include "ComPrivate.h"

#define SignalLostTimeout 5*1000

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

bool PassBack_State = OFF;

void NRF_Init()
{
Reinit:
    nrf.SetAddress(NRF_AddressConfig);
    nrf.SetSpeed(nrf.SPEED_250Kbps);
	nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    if(!nrf.IsDetect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(200);
        goto Reinit;
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
    if(!ConnectState.IsConnect)
    {
        __IntervalExecute(
            nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff)),
            100
        );
    }
    
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
    
    if(millis() - ConnectState.TimePoint_Recv > SignalLostTimeout)
    {
        NVIC_SystemReset();
    }
}
