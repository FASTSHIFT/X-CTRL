#include "NRF.h"

/**
  * @brief  发送缓冲区数据,接收数据到接收缓冲区(半双工通信，用于主机，适用于信号较差的环境)
  * @param  *txbuff:发送缓冲区地址
  * @param  *rxbuff:接收缓冲区地址
  * @retval 无
  */
void NRF_TRM::TranRecv(uint8_t *txbuff, uint8_t *rxbuff)
{
    uint8_t status = Basic->GetStatus();
    if(Basic->RF_State == Basic->State_TX)
    {
        if(status & Basic->TX_DS)
        {
            Basic->TranSuccess_Cnt++;
            Basic->RX_Mode();
        }
        else
        {
            Basic->TX_Mode();
            Basic->Tran(txbuff);
        }
    }
    else if(Basic->RF_State == Basic->State_RX)
    {
        Basic->Recv(rxbuff);
        Basic->TX_Mode();
        Basic->Tran(txbuff);
    }
}

/**
  * @brief  接收数据到接收缓冲区,发送缓冲区数据(半双工通信，用于从机，适用于信号较差的环境)
  * @param  *rxbuff:接收缓冲区地址
  * @param  *txbuff:发送缓冲区地址
  * @retval 无
  */
void NRF_TRM::RecvTran(uint8_t *rxbuff, uint8_t *txbuff)
{
    uint8_t status = Basic->GetStatus();
    if(Basic->RF_State == Basic->State_RX)
    {
        if(Basic->Recv(rxbuff))
        {
            Basic->TX_Mode();
            Basic->Tran(txbuff);
        }
        else
        {
            Basic->RX_Mode();
        }
    }
    else if(Basic->RF_State == Basic->State_TX)
    {
        if(status & Basic->TX_DS)
        {
            Basic->TranSuccess_Cnt++;
        }
        Basic->RX_Mode();
    }
}
