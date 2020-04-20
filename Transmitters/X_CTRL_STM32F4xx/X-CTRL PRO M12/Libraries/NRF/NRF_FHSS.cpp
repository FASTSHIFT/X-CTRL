#include "NRF.h"

/**
  * @brief  跳频处理
  * @param  无
  * @retval 无
  */
void NRF_FHSS::FH_Process()
{
    FH_List_Index++;
    FH_List_Index %= FH_List_Length;
    Basic->SetFreqency(FH_List[FH_List_Index]);
}

/**
  * @brief  跳频+单工发送
  * @param  txbuff:发送数据包地址
  * @retval 无
  */
void NRF_FHSS::TxProcess(void* txbuff)
{
    /*发射端先跳频*/
    FH_Process();
    /*检查上一次的发送情况*/
    Basic->TranCheck();
    /*再发送*/
    Basic->Tran(txbuff);
    /*不管接收端状态*/
}

/**
  * @brief  跳频+单工接收
  * @param  rxbuff:接收数据包地址
  * @retval 无
  */
void NRF_FHSS::RxProcess(void* rxbuff)
{
    /*如果正常接收，则下一次正常跳频*/
    if(Basic->Recv(rxbuff))
    {
        LastRxTime = millis();
        FH_Enable = true;
    }

    /*是否启用正常跳频*/
    if(FH_Enable)
    {
        FH_Process();
    }

    /*在丢失信号超时后，接收端关闭正常跳频*/
    /*慢跳等待重新同步发射端，慢跳周期大于发端遍历跳频表的周期*/
    /*一旦接收成功，立即启动正常跳频*/
    if(millis() - LastRxTime >= InterruptTime * (FH_List_Length + 5))
    {
        LastRxTime = millis();
        FH_Process();
        FH_Enable = false;
    }
}

/**
  * @brief  跳频+半双工(发送端)
  * @param  txbuff:发送数据包地址
  * @param  rxbuff:接收数据包地址(如果想单工就传入NULL)
  * @retval 无
  */
void NRF_FHSS::TxProcess(void* txbuff, void* rxbuff)
{
    /*是否使用单工*/
    if(rxbuff == NULL)
    {
        TxProcess(txbuff);
        return;
    }
    
    uint8_t status = Basic->GetStatus();
    if(Basic->RF_State == Basic->State_RX)
    {
        Basic->Recv(rxbuff);
        FH_Process();
        Basic->TX_Mode();
        Basic->Tran(txbuff);
    }
    else if(Basic->RF_State == Basic->State_TX)
    {
        if(status & Basic->TX_DS)
        {
            Basic->RF_TranSuccessCnt++;
        }
        Basic->RX_Mode();
    }
}

/**
  * @brief  跳频+半双工(接收端)
  * @param  rxbuff:接收数据包地址
  * @param  txbuff:发送数据包地址(如果想单工就传入NULL)
  * @retval 无
  */
void NRF_FHSS::RxProcess(void* rxbuff, void* txbuff)
{
    if(txbuff == NULL)
    {
        RxProcess(rxbuff);
        return;
    }
    
    uint8_t status = Basic->GetStatus();
    if(Basic->RF_State == Basic->State_TX)
    {
        if(status & Basic->TX_DS)
        {
            Basic->RF_TranSuccessCnt++;
        }
        
        if(FH_Enable)
        {
            FH_Process();
        }
        Basic->RX_Mode();
    }
    else if(Basic->RF_State == Basic->State_RX)
    {
        if(Basic->Recv(rxbuff))
        {
            LastRxTime = millis();
            FH_Enable = true;
            Basic->TX_Mode();
            Basic->Tran(txbuff);
        }
        else
        {
            if(millis() - LastRxTime >= InterruptTime * (FH_List_Length + 5))
            {
                LastRxTime = millis();
                FH_Process();
                FH_Enable = false;
            }
            Basic->RX_Mode();
        }
    }
}
