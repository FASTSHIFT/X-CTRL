#include "FileGroup.h"
#include "ComPrivate.h"
#include "SpeedCtrl.h"

#define ConnectLost_TimeOut 500
//#define SignalLostTimeout (6*1000)
#define __ADCToVoltage(x) (x/4095.0*3.3*11.0)
#define RC_X_Temp    (*((Protocol_CtrlType*)NRF_RxBuff))
#define PassbackData (*((Protocol_Passback_CommonDisplay_t*)NRF_TxBuff))
#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

uint8_t NRF_RxBuff[32];
uint8_t NRF_TxBuff[32];

Protocol_CtrlType RC_X = {0};
ConnectState_t ConnectState = {0};
bool PassBack_State = ON;

NRF_Config_TypeDef NRF_Cfg;

void Init_NRF()
{
    StorageDataReg(NRF_Cfg);
    EEPROM_Handle(EEPROM_Chs::ReadData);
    memset(NRF_RxBuff, 0xff, sizeof(NRF_RxBuff));
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
        delay(100);
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
    ReportState %= 4;
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
        PassbackData.X = 13;
        PassbackData.Y = 40;
        PassbackData.TextSize = 2;
        PassbackData.TextColor = 0xFFFF;
        PassbackData.BackColor = 0x0000;
        sprintf(PassbackData.Text, "%04.1fV", __ADCToVoltage(analogRead(BattDet_Pin)));
        break;
    case 2:
        PassbackData.X = 80;
        PassbackData.Y = 35;
        PassbackData.TextSize = 1;
        PassbackData.TextColor = 0xFFFF;
        PassbackData.BackColor = 0x0000;
        sprintf(PassbackData.Text, "%04.1f", ElsSpeed);
        break;
    case 3:
        PassbackData.X = 80;
        PassbackData.Y = 45;
        PassbackData.TextSize = 1;
        PassbackData.TextColor = 0xFFFF;
        PassbackData.BackColor = 0x0000;
        sprintf(PassbackData.Text, "%04.1f", ElsDistance);
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
    __EventMonitor(src.Info.DataPackage_Num, state.TimePoint_Recv = millis());

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
    static bool IsIWDGEnable = false;
    if(PassBack_State)
        nrf.RecvTranSafe(NRF_RxBuff, NRF_TxBuff);
    else
        nrf.Recv(NRF_RxBuff);

#ifdef USE_WATCH_DOG
    if(RxDataVerify(RC_X_Temp, ConnectState, ConnectLost_TimeOut) == true)
    {
        RC_X = RC_X_Temp;
        if(!IsIWDGEnable)
        {
            Init_IWDG(IWDG_Prescaler_64, 625);
            IsIWDGEnable = true;
        }
    }
#endif

#ifdef USE_POWER_DOWN
    if(ElsSpeed < 1 && millis() - ConnectState.TimePoint_Recv > SignalLostTimeout)
    {
        if(IsIWDGEnable)
        {
            NVIC_SystemReset();
        }
        else
        {
            EnterSystemPowerMode();
        }
    }
#endif
}

