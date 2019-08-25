#include "FileGroup.h"
#include "ComPrivate.h"

#define __ADCToVoltage(x) (x/4095.0*3.3*11.0)
#define RC_X_Temp    (*((Protocol_CtrlType*)NRF_RxBuff))
#define PassbackData (*((Protocol_Passback_CommonDisplay_t*)NRF_TxBuff))

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];

Protocol_CtrlType RC_X = {0};
ConnectState_t ConnectState = {0};
bool State_PassBack = ON;

NRF_Config_TypeDef NRF_Cfg;

void Init_NRF()
{
ReInit:
    nrf.SetAddress(NRF_Cfg.Address);
    nrf.SetFreqency(NRF_Cfg.Freq);
    if(NRF_Cfg.Speed == 0)
        nrf.SetSpeed(nrf.SPEED_250Kbps);
    else if(NRF_Cfg.Speed == 1)
        nrf.SetSpeed(nrf.SPEED_1Mbps);
    else if(NRF_Cfg.Speed == 2)
        nrf.SetSpeed(nrf.SPEED_2Mbps);
    
    nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));

    if(!nrf.IsDetect())
    {
        togglePin(LED_Pin);
        delay(50);
        goto ReInit;
    }
    nrf.ClearFlag();

    PassbackData.X = 15;
    PassbackData.Y = 27;
    PassbackData.TextSize = 1;
    PassbackData.TextColor = 0xF800;
    PassbackData.BackColor = 0x0000;
    strcpy(PassbackData.Text, Slave_Description);
}

void Task_InfoReport()
{
	if(!State_PassBack)
		return;
	
    static uint8_t ReportState = 0;
    __ValueStep(ReportState, 1, 1);
	
    switch(ReportState)
    {
    case 0:
        PassbackData.X = 18;
        PassbackData.Y = 25;
        PassbackData.TextSize = 1;
        PassbackData.TextColor = 0xF800;
        PassbackData.BackColor = 0x0000;
        strcpy(PassbackData.Text, Slave_Description);
        break;
    case 1:
//        PassbackData.X = 13;
//        PassbackData.Y = 40;
//        PassbackData.TextSize = 2;
//        PassbackData.TextColor = 0xFFFF;
//        PassbackData.BackColor = 0x0000;
//        sprintf(PassbackData.Text, "%04.1fV", __ADCToVoltage(analogRead(BattDet_Pin)));
        break;
    }
}

/**
  * @brief  NRF数据校验
  * @param  Protocol_CtrlType: 当前控制对象结构体
  * @param  ConnectState_t: 连接状态结构体
  * @retval true: 校验成功, false:校验失败
  */
static bool RxDataVerify(Protocol_CtrlType &src, ConnectState_t &state, uint32_t timeout)
{
    __ValueMonitor(src.Info.DataPackage_Num, state.TimePoint_Recv = millis());

    if(millis() - state.TimePoint_Recv < timeout)//连接超时判断
        state.IsConnect = true;
    else
        state.IsConnect = false;

    if(src.Info.CtrlObject == Slave_Type)//控制对象认证
        state.IsCorrect = true;
    else
        state.IsCorrect = false;

    return (state.IsConnect && state.IsCorrect);
}

void Task_NRF_Handler()
{
    if(State_PassBack)
        nrf.RecvTranSafe(NRF_RxBuff, NRF_TxBuff);
    else
        nrf.Recv(NRF_RxBuff);
    
    if(RxDataVerify(RC_X_Temp, ConnectState, ConnectLost_TimeOut) == true)
    {
        RC_X = RC_X_Temp; 
    }

#ifndef HandshakeEnable_Pin
    if(millis() - ConnectState.TimePoint_Recv > SignalLostTimeout)
        NVIC_SystemReset();
#endif
}

