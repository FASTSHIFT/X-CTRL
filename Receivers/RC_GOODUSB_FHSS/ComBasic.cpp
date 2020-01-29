#include "FileGroup.h"
#include "ComPrivate.h"

#define ConnectLost_TimeOut 500
#define SignalLostTimeout (6*1000)
#define RC_X_Temp    (*((Protocol_CtrlType*)NRF_RxBuff))
#define PassbackData (*((Protocol_Passback_CommonDisplay_t*)NRF_TxBuff))

NRF_Basic nrf(
  NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, 
  NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND
NRF_TRM  nrfTRM(&nrf);
NRF_FHSS nrfFHSS(&nrf);

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];

Protocol_CtrlType RC_X = {0};
ConnectState_t ConnectState = {0};
bool State_PassBack = Slave_SupportPassback;
bool State_FHSS     = Slave_SupportFHSS;

NRF_Config_TypeDef NRF_Cfg;

void Init_NRF()
{
ReInit:
  nrf.Init();
  nrf.SetRF_Enable(false);
  nrf.SetAddress(NRF_Cfg.Address);
  nrf.SetFreqency(NRF_Cfg.Freq);
  nrf.SetSpeed(NRF_Cfg.Speed);
  nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
  nrf.SetRF_Enable(true);

  if (!nrf.IsDetect())
  {
    togglePin(LED_Pin);
    goto ReInit;
  }

  PassbackData.X = 15;
  PassbackData.Y = 27;
  PassbackData.TextSize = 1;
  PassbackData.TextColor = 0xF800;
  PassbackData.BackColor = 0x0000;
  strcpy(PassbackData.Text, Slave_Description);
}

void Task_InfoReport()
{
  static uint8_t ReportState = 0;
  ReportState ++;
  ReportState %= 1;
  switch (ReportState)
  {
    case 0:
      PassbackData.X = 18;
      PassbackData.Y = 25;
      PassbackData.TextSize = 1;
      PassbackData.TextColor = 0xF800;
      PassbackData.BackColor = 0x0000;
      strcpy(PassbackData.Text, Slave_Description);
      break;
  }
}

/**
    @brief  NRF数据校验
    @param  Protocol_CtrlType: 当前控制对象结构体
    @param  ConnectState_t: 连接状态结构体
    @retval true: 校验成功, false:校验失败
*/
static bool RxDataVerify(Protocol_CtrlType &src, ConnectState_t &state, uint32_t timeout)
{
  __EventMonitor(src.Info.DataPackage_Num, state.TimePoint_Recv = millis());

  if (millis() - state.TimePoint_Recv < timeout) //连接超时判断
    state.IsConnect = true;
  else
    state.IsConnect = false;

  if (src.Info.CtrlObject == Slave_Type) //控制对象认证
    state.IsCorrect = true;
  else
    state.IsCorrect = false;

  return (state.IsConnect && state.IsCorrect);
}

void Task_NRF_Handler()
{
  if(State_FHSS || State_PassBack)
  {
    if(State_FHSS)
    {
      nrfFHSS.RxProcess(
        NRF_RxBuff, 
        State_PassBack ? NRF_TxBuff : NULL
      );
    }
    else
    {
      nrfTRM.RecvTran(NRF_RxBuff, NRF_TxBuff);
    }
  }
  else
  {
    if(nrf.GetRF_State() != nrf.State_RX)
    {
      nrf.RX_Mode();
    }
    nrf.Recv(NRF_RxBuff);
  }

  if (RxDataVerify(RC_X_Temp, ConnectState, ConnectLost_TimeOut) == true)
  {
    RC_X = RC_X_Temp;
  }

  if (millis() - ConnectState.TimePoint_Recv >= SignalLostTimeout)
    SystemReset();
}
