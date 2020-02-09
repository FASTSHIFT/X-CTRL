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

/*通信使能*/
static bool Com_Enable = OFF;

void Com_SetRFEnable(bool en)
{
    Com_Enable = en;
    MainTask.TaskStateCtrl(Task_TransferData, en);
}

bool Com_GetRFEnable()
{
    return Com_Enable;
}

/**
  * @brief  初始化默认通道配置
  * @param  无
  * @retval 无
  */
void Com_SetDefaultChannel()
{
    RCX::ChannelSetAttachEnable(true);
    
    RCX::ChannelSetAttach(0, &CTRL.KnobLimit.L);
    RCX::ChannelSetAttach(1, &CTRL.KnobLimit.R);
    RCX::ChannelSetAttach(2, NULL);
    RCX::ChannelSetAttach(3, NULL);
    RCX::ChannelSetAttach(4, &CTRL.JS_L.X.Val);
    RCX::ChannelSetAttach(5, &CTRL.JS_L.Y.Val);
    RCX::ChannelSetAttach(6, &CTRL.JS_R.X.Val);
    RCX::ChannelSetAttach(7, &CTRL.JS_R.Y.Val);
    RCX::ChannelSetAttach(8, NULL);
    RCX::ChannelSetAttach(9, NULL);
    RCX::ChannelSetAttach(10, NULL);
    RCX::ChannelSetAttach(11, NULL);
}

/**
  * @brief  通信初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Com_Init()
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
    Com_SetDefaultChannel();

    /*返回连接情况*/
    return isInit;
}

static void Com_TxRxProcess()
{
    if(CTRL.State.FHSS)
    {
        nrfFHSS.TxProcess(
            NRF_TxBuff,
            CTRL.State.PassBack ? NRF_RxBuff : NULL
        );
    }
    else
    {
        nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);
    }

    if(CTRL.State.PassBack)
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
    if(!Com_Enable)
        return;
    
    /*写入按键状态*/
    RCX::SetPackKey(CTRL.Key.Value);
    /*载入数据包到发送缓冲区*/
    RCX::LoadPack(NRF_TxBuff);

    if(CTRL.State.FHSS || CTRL.State.PassBack)
    {
        Com_TxRxProcess();
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
