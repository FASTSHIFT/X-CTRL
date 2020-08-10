#include "Basic/FileGroup.h"
#include "ComPrivate.h"
#include "BSP/BSP.h"

/*实例化NRF对象*/
NRF_Basic nrf(
    NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin,
    NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

/*定频收发管理器*/
NRF_TRM  nrfTRM(&nrf);

/*跳频收发管理器*/
NRF_FHSS nrfFHSS(&nrf);

/*NRF发送数据缓冲区*/
static uint8_t NRF_TxBuff[32];

/*NRF接收数据缓冲区*/
static uint8_t NRF_RxBuff[32];

/*通信使能*/
static bool Com_Enable = false;

void Com_SetEnable(bool en)
{
    Com_Enable = en;
    Com_PassbackProcess(en ? PBS_Setup : PBS_Exit);
}

bool Com_GetEnable()
{
    return Com_Enable;
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

    /*数据包长度*/
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    
    /*设置速率*/
    nrf.SetSpeed(CTRL.RF_Config->Speed);
    
    /*设置频率*/
    nrf.SetFreqency(CTRL.RF_Config->Freq);
    
    /*设置地址*/
    nrf.SetAddress(CTRL.RF_Config->Addr);

    /*禁用射频*/
    nrf.SetRF_Enable(false);

    /*更新寄存器状态*/
    nrf.UpdateRegs();

    /*返回连接情况*/
    return isInit;
}

static void Com_TxRxProcess()
{
    if(CTRL.State->FHSS)
    {
        nrfFHSS.TxProcess(
            NRF_TxBuff,
            CTRL.State->Passback ? NRF_RxBuff : NULL
        );
    }
    else
    {
        nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);
    }

    if(CTRL.State->Passback)
    {
        RCX::RxLoadPack(NRF_RxBuff);
        Com_PassbackProcess(RCX::RxGetSignalLost() ? PBS_Error : PBS_Loop);
    }
}

/**
  * @brief  数据发送任务
  * @param  无
  * @retval 无
  */
void Com_Update()
{
    if(!Com_Enable)
        return;
    
    /*写入按键状态*/
    RCX::TxSetPackKey(CTRL.Key.Value);
    /*载入数据包到发送缓冲区*/
    RCX::TxLoadPack(NRF_TxBuff);

    if(CTRL.State->FHSS || CTRL.State->Passback)
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
