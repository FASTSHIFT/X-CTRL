#include "FileGroup.h"
#include "ComPrivate.h"

static NRF_Basic nrf(
    NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin,
    NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND
static NRF_TRM  nrfTRM(&nrf);
static NRF_FHSS nrfFHSS(&nrf);

static uint8_t NRF_RxBuff[32];
static uint8_t NRF_TxBuff[32];

void Com_Init()
{
ReInit:
    nrf.Init();
    nrf.SetRF_Enable(false);
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    nrf.SetRF_Enable(true);

    if (!nrf.IsDetect())
    {
        togglePin(LED_Pin);
        goto ReInit;
    }
    
    RCX::Handshake::Init(&nrfTRM, &nrfFHSS);
}

void Task_ComHandler()
{
    if(RCX::Handshake::GetPassBack_Enable())
    {
        RCX::LoadTxPack(NRF_TxBuff);
        if(RCX::Handshake::GetFHSS_Enable())
        {
            nrfFHSS.RxProcess(NRF_RxBuff, NRF_TxBuff);
        }
        else
        {
            nrfTRM.RecvTran(NRF_RxBuff, NRF_TxBuff);
        }
        
        __IntervalExecute(nrf.GetRSSI(), 2000);
    }
    else
    {
        if(RCX::Handshake::GetFHSS_Enable())
        {
            nrfFHSS.RxProcess(NRF_RxBuff);
        }
        else
        {
            if(nrf.GetRF_State() != nrf.State_RX)
            {
                nrf.RX_Mode(true);
            }
            nrf.Recv(NRF_RxBuff);
        }
    }
    
    RCX::LoadRxPack(NRF_RxBuff);

    if (RCX::GetRxSignalLost())
        NVIC_SystemReset();
}
