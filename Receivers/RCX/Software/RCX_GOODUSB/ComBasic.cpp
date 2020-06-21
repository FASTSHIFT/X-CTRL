#include "FileGroup.h"
#include "ComPrivate.h"

NRF_Basic nrf(
  NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin,
  NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND
static NRF_TRM  nrfTRM(&nrf);
static NRF_FHSS nrfFHSS(&nrf);

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];

static const char* SlaveName = "GOODUSB";

void Com_SetDescription(const char* name)
{
  SlaveName = name;
}

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

  RCX::Handshake::Init(&nrfTRM, &nrfFHSS, SlaveName);
}

void Task_ComHandler()
{
    if(RCX::Handshake::GetPassBack_Enable())
    {
        RCX::TxLoadPack(NRF_TxBuff);
        if(RCX::Handshake::GetFHSS_Enable())
        {
            nrfFHSS.RxProcess(NRF_RxBuff, NRF_TxBuff);
        }
        else
        {
            nrfTRM.RecvTran(NRF_RxBuff, NRF_TxBuff);
        }
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
                nrf.RX_Mode();
            }
            nrf.Recv(NRF_RxBuff);
        }
    }
    
    RCX::RxLoadPack(NRF_RxBuff);

  if (RCX::RxGetSignalLost())
    NVIC_SystemReset();
}
