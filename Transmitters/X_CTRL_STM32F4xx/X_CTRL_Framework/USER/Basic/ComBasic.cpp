#include "FileGroup.h"
#include "ComPrivate.h"

/*实例化NRF对象*/
NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

/*NRF发送数据缓冲区*/
uint8_t NRF_TxBuff[32];

/*NRF接收数据缓冲区*/
uint8_t NRF_RxBuff[32];

/*发送数据使能*/
bool State_RF = OFF;

/*回传使能*/
bool State_PassBack = OFF;

/*握手使能*/
bool State_Handshake = ON;

/**
  * @brief  NRF初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Init_NRF()
{
    /*总初始化*/
    nrf.init(nrf.TXRX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));

    /*返回连接情况*/
    return nrf.IsDetect();
}

/**
  * @brief  数据发送任务
  * @param  无
  * @retval 无
  */
void Task_TransferData()
{
    if(State_RF == OFF) return;
}
