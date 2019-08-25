#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];
Protocol_Common_t RC_Aircraft;
ConnectState_t ConnectState;

bool PassBack_State = OFF;

void NRF_Init()
{
	if(PassBack_State)
		nrf.init(RXTX_MODE, (uint8_t*)&NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
	else
		nrf.init(RX_MODE, sizeof(NRF_RxBuff));
    while(!nrf.IsDetect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
    }
}

void NRF_CheckConnect(Protocol_Common_t* rc_x)
{
    static uint8_t DataPackage_Num_Last;
    if (rc_x->Info.DataPackage_Num != DataPackage_Num_Last)
    {
        ConnectState.TimePoint_Recv = millis();
        DataPackage_Num_Last = rc_x->Info.DataPackage_Num;
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
    static Protocol_Common_t RC_X_Temp;
	if(PassBack_State)
	{
//		memcpy(NRF_TxBuff,&CarSS_Slave,sizeof(CarSS_Slave));
//		nrf.RecvTran(NRF_RxBuff,NRF_TxBuff);
	}
	else
	{
		nrf.Recv(NRF_RxBuff);
	}
	memcpy(&RC_X_Temp, NRF_RxBuff, sizeof(RC_X_Temp));

    NRF_CheckConnect(&RC_X_Temp);

    if(RC_X_Temp.Info.CtrlObject == X_COMMON)
    {
        ConnectState.IsCorrect = true;
        RC_Aircraft = RC_X_Temp;
    }
    else
    {
        ConnectState.IsCorrect = false;
    }
}

