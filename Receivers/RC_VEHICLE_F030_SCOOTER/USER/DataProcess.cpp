#include "FileGroup.h"
#include "stdio.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

static uint8_t NRF_RxBuff[32];
static uint8_t NRF_TxBuff[32];

Protocol_CtrlType RC_X;
Protocol_Passback_CommonDisplay_t *PassbackData = ( Protocol_Passback_CommonDisplay_t *)&NRF_TxBuff;
ConnectState_t ConnectState;
#define USE_PassBack

char* strcpy(char* des, const char* source)
{
    char* r = des;
    while((*r++ = *source++) != '\0');
    return des;
}


void NRF_Init()
{
    nrf.SetAddress(NRF_AddressConfig);
    nrf.SetFreqency(NRF_FreqConfig);
    nrf.SetSpeed(NRF_SpeedConfig);
#ifdef USE_PassBack
        nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
#else
        nrf.init(nrf.RX_MODE, 0, 0, sizeof(NRF_RxBuff));
#endif
    while(!nrf.IsDetect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
    }
    PassbackData->X = 15;
    PassbackData->Y = 27;
    PassbackData->TextSize = 1;
    PassbackData->TextColor = 0xF800;
    PassbackData->BackColor = 0x0000;
    strcpy(PassbackData->Text, "eScooter");
}

void Task_InfoReport()
{
    static uint8_t ReportState = 0;
    ReportState ++;
    ReportState %= 2;
    if(ReportState == 0)
    {
        PassbackData->X = 15;
        PassbackData->Y = 27;
        PassbackData->TextSize = 1;
        PassbackData->TextColor = 0xF800;
        PassbackData->BackColor = 0x0000;
        strcpy(PassbackData->Text, "eScooter");
    }
    else if(ReportState == 1)
    {
#define toVoltage(x) (x*3.3/4095*7.75)
        PassbackData->X = 13;
        PassbackData->Y = 40;
        PassbackData->TextSize = 2;
        PassbackData->TextColor = 0xF800;
        PassbackData->BackColor = 0x0000;
        sprintf(PassbackData->Text, "%04.1fV", toVoltage(analogRead(BattDet_Pin)));
    }
}

/**
  * @brief  NRF数据校验
  * @param  Protocol_CtrlType: 当前控制对象结构体指针
  * @param  ConnectState_t: 连接状态结构体指针
  * @retval true: 校验成功, false:校验失败
  */
static bool DataVerify(Protocol_CtrlType *src, ConnectState_t *state, uint32_t timeout)
{
    static uint8_t DataPackage_Num_Last;//上一次的心跳包
    if (src->Info.DataPackage_Num != DataPackage_Num_Last)//心跳包验证
    {
        state->TimePoint_Recv = millis();//标记时间点，用于超时判断
        DataPackage_Num_Last = src->Info.DataPackage_Num;
    }

    if(millis() - state->TimePoint_Recv < timeout)//连接超时判断
        state->IsConnect = true;
    else
        state->IsConnect = false;

    if(src->Info.CtrlObject == NowCtrlObject)//控制对象认证
        state->IsCorrect = true;
    else
        state->IsCorrect = false;

    return (state->IsConnect && state->IsCorrect);
}

void Task_NRF_Receive()
{
#ifdef USE_PassBack
        nrf.RecvTran(NRF_RxBuff, NRF_TxBuff);
#else
        nrf.Recv(NRF_RxBuff);
#endif

    Protocol_CtrlType RC_X_Temp = *(Protocol_CtrlType*)&NRF_RxBuff;

    if(DataVerify(&RC_X_Temp, &ConnectState, ConnectLost_TimeOut) == true)
    {
        RC_X = RC_X_Temp;
    }
}

