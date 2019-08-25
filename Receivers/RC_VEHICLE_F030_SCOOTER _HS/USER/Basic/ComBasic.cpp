#include "FileGroup.h"
#include "ComPrivate.h"
#include "stdio.h"
#include "string.h"

#define SignalLostTimeout (10*1000)
#define __ADCToVoltage(x) (x*3.3/4095*7.75)
#define RC_X_Temp    (*((Protocol_CtrlType*)NRF_RxBuff))
#define PassbackData (*((Protocol_Passback_CommonDisplay_t*)NRF_TxBuff))

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];

Protocol_CtrlType RC_X;
ConnectState_t ConnectState;
bool PassBack_State = ON;

void NRF_Init()
{
ReInit:
    nrf.SetAddress(NRF_AddressConfig);
    nrf.SetFreqency(NRF_FreqConfig);
    nrf.SetSpeed(NRF_SpeedConfig);
    nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    if(!nrf.IsDetect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
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
    ReportState %= 2;
    if(ReportState == 0)
    {
        PassbackData.X = 15;
        PassbackData.Y = 27;
        PassbackData.TextSize = 1;
        PassbackData.TextColor = 0xF800;
        PassbackData.BackColor = 0x0000;
        strcpy(PassbackData.Text, Slave_Description);
    }
    else if(ReportState == 1)
    {
        PassbackData.X = 13;
        PassbackData.Y = 40;
        PassbackData.TextSize = 2;
        PassbackData.TextColor = 0xF800;
        PassbackData.BackColor = 0x0000;
        sprintf(PassbackData.Text, "%04.1fV", __ADCToVoltage(analogRead(BattDet_Pin)));
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
    __EventMonitor(src.Info.DataPackage_Num, state.TimePoint_Recv = millis());

    if(millis() - state.TimePoint_Recv < timeout)//连接超时判断
        state.IsConnect = true;
    else
        state.IsConnect = false;

    if(src.Info.CtrlObject == NowCtrlObject)//控制对象认证
        state.IsCorrect = true;
    else
        state.IsCorrect = false;

    return (state.IsConnect && state.IsCorrect);
}

void Task_NRF_Handler()
{
//    if(!ConnectState.IsConnect)
//    {
//        __IntervalExecute(
//            nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff)),
//            100
//        );
//        //__EventMonitor(ConnectState.IsConnect, tone(Buzz_Pin, 1000, 200));
//    }

    if(PassBack_State)
        nrf.RecvTranSafe(NRF_RxBuff, NRF_TxBuff);
    else
        nrf.Recv(NRF_RxBuff);

    if(RxDataVerify(RC_X_Temp, ConnectState, ConnectLost_TimeOut) == true)
        RC_X = RC_X_Temp;

    if(millis() - ConnectState.TimePoint_Recv > SignalLostTimeout)
        NVIC_SystemReset();
}
