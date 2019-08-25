#include "FileGroup.h"
#include "stdio.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];
Protocol_CtrlType RC_X;
Protocol_CtrlType* RC_X_RxBuff = (Protocol_CtrlType*)&NRF_RxBuff;
Protocol_Passback_CommonDisplay_t *Common_Passback = ( Protocol_Passback_CommonDisplay_t *)&NRF_TxBuff;
ConnectState_t ConnectState;
bool PassBack_State = OFF;

char* strcpy(char* des, const char* source)
{
    char* r = des;
    while((*r++ = *source++) != '\0');
    return des;
}


void NRF_Init()
{   
    nrf.SetSpeed(nrf.SPEED_1Mbps);
    nrf.SetAddress(NRF_AddressConfig);
    if(PassBack_State)
        nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    else
        nrf.init(nrf.RX_MODE, 0, 0, sizeof(NRF_RxBuff));
    while(!nrf.IsDetect())
    {
        analogWrite(LED_Pin, 1000);
        delay(100);
        analogWrite(LED_Pin, 0);
        delay(100);
    }
    Common_Passback->X = 15;
    Common_Passback->Y = 27;
    Common_Passback->TextSize = 1;
    Common_Passback->TextColor = 0xF800;
    Common_Passback->BackColor = 0x0000;
    strcpy(Common_Passback->Text, "XSuperWand");
}

void NRF_CheckConnect(Protocol_CtrlType* rc_x)
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
    static Protocol_CtrlType RC_X_Temp;
    if(PassBack_State)
    {
        nrf.RecvTran(NRF_RxBuff, NRF_TxBuff);
    }
    else
    {
        nrf.Recv(NRF_RxBuff);
    }
    RC_X_Temp = *RC_X_RxBuff;

    NRF_CheckConnect(&RC_X_Temp);

    if(RC_X_Temp.Info.CtrlObject == NowCtrlObject)
    {
        ConnectState.IsCorrect = true;
        RC_X = RC_X_Temp;
    }
    else
    {
        ConnectState.IsCorrect = false;
    }
}

