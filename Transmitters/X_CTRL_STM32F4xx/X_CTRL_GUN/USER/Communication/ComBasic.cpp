#include "FileGroup.h"
#include "ComPrivate.h"

/*失联超时500ms*/
#define ConnectLost_TimeOut 500

/*实例化NRF对象*/
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

/*控制器*/
CTRL_TypeDef CTRL;

/*NRF发送数据缓冲区*/
static uint8_t NRF_TxBuff[32];

/*NRF接收数据缓冲区*/
static uint8_t NRF_RxBuff[32];

/*NRF地址分配表*/
uint8_t NRF_AddressConfig[] =
{
    45, 23, 78, 19, 61, //ADDR:0
    75, 75, 75, 75, 75, //ADDR:1
    12, 34, 56, 78, 90, //ADDR:2
    12, 90, 17, 44, 55, //ADDR:3
    10, 93, 70, 72, 31, //ADDR:4
    0,  0,   0,  0,  0 //ADDR: Handshake
};

/*NRF信号强度指示*/
int16_t NRF_SignalStrength = 0;

/*NRF基本配置表*/
NRF_Config_TypeDef NRF_Cfg = {0, 0, 40};

/*发送数据使能*/
bool State_RF = OFF;

/*回传使能*/
bool State_PassBack = ON;

/*通用回传模式使能*/
bool Enable_CommonPassBack = true;

/*握手使能*/
bool State_Handshake = ON;

/*跳频使能*/
bool State_FHSS = OFF;

/**
  * @brief  初始化默认通道配置
  * @param  无
  * @retval 无
  */
void Init_DefaultChannel()
{
    RCX::ChannelAttachValueSetEnable(true);
    
    RCX::ChannelAttachValue(0, &CTRL.KnobLimit.L);
    RCX::ChannelAttachValue(1, &CTRL.KnobLimit.R);
    RCX::ChannelAttachValue(2, &CTRL.KnobCab.L);
    RCX::ChannelAttachValue(3, &CTRL.KnobCab.R);
    RCX::ChannelAttachValue(4, &CTRL.Left.X);
    RCX::ChannelAttachValue(5, &CTRL.Left.Y);
    RCX::ChannelAttachValue(6, &CTRL.Right.X);
    RCX::ChannelAttachValue(7, &CTRL.Right.Y);
    RCX::ChannelAttachValue(8, NULL);
    RCX::ChannelAttachValue(9, NULL);
    RCX::ChannelAttachValue(10, NULL);
    RCX::ChannelAttachValue(11, NULL);
}

/**
  * @brief  NRF初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Init_NRF()
{
    /*默认初始化*/
    bool isInit = nrf.Init();

    /*发送模式*/
    nrf.TX_Mode();

    /*设置地址*/
    nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);

    /*设置频率*/
    nrf.SetFreqency(NRF_Cfg.Freq);

    /*设置速率*/
    nrf.SetSpeed(NRF_Cfg.Speed);

    /*数据包长度*/
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));

    /*禁用射频*/
    nrf.SetRF_Enable(false);

    /*更新寄存器状态*/
    nrf.UpdateRegs();

    /*初始化默认通道*/
    Init_DefaultChannel();

    /*返回连接情况*/
    return isInit;
}

void NRF_TxRx_Process()
{
    if(State_FHSS)
    {
        nrfFHSS.TxProcess(
            NRF_TxBuff,
            State_PassBack ? NRF_RxBuff : NULL
        );
    }
    else
    {
        nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);
    }

    if(State_PassBack)
    {
        RCX::PassbackProcess(NRF_RxBuff);
    }
}

/**
  * @brief  数据发送任务
  * @param  无
  * @retval 无
  */
void Task_TransferData()
{
    if(State_RF == OFF) return;
    
    /*写入按键状态*/
    RCX::SetPackKey(CTRL.Key.Value);
    /*载入数据包到发送缓冲区*/
    RCX::LoadPack(NRF_TxBuff);

    if(State_FHSS || State_PassBack)
    {
        NRF_TxRx_Process();
    }
    else
    {
        if(nrf.GetRF_State() != nrf.State_TX)
        {
            nrf.TX_Mode();
        }
        nrf.TranCheck();
        nrf.Tran(NRF_TxBuff);//NRF发送数据
    }
}
