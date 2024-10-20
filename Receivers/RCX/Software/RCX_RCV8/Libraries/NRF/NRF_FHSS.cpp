#include "NRF.h"

/**
  * @brief  ��Ƶ����
  * @param  ��
  * @retval ��
  */
void NRF_FHSS::FH_Process()
{
    FH_List_Index++;
    FH_List_Index %= FH_List_Length;
    Basic->SetFreqency(FH_List[FH_List_Index]);
}

/**
  * @brief  ��Ƶ+��������
  * @param  txbuff:�������ݰ���ַ
  * @retval ��
  */
void NRF_FHSS::TxProcess(void* txbuff)
{
    /*���������Ƶ*/
    FH_Process();
    /*�����һ�εķ������*/
    Basic->TranCheck();
    /*�ٷ���*/
    Basic->Tran(txbuff);
    /*���ܽ��ն�״̬*/
}

/**
  * @brief  ��Ƶ+��������
  * @param  rxbuff:�������ݰ���ַ
  * @retval ��
  */
void NRF_FHSS::RxProcess(void* rxbuff)
{
    /*����������գ�����һ��������Ƶ*/
    if(Basic->Recv(rxbuff))
    {
        LastRxTime = millis();
        FH_Enable = true;
    }

    /*�Ƿ�����������Ƶ*/
    if(FH_Enable)
    {
        FH_Process();
    }

    /*�ڶ�ʧ�źų�ʱ�󣬽��ն˹ر�������Ƶ*/
    /*�����ȴ�����ͬ������ˣ��������ڴ��ڷ��˱�����Ƶ�������*/
    /*һ�����ճɹ�����������������Ƶ*/
    if(millis() - LastRxTime >= InterruptTime * (FH_List_Length + 1))
    {
        LastRxTime = millis();
        FH_Process();
        FH_Enable = false;
    }
}

/**
  * @brief  ��Ƶ+��˫��(���Ͷ�)
  * @param  txbuff:�������ݰ���ַ
  * @param  rxbuff:�������ݰ���ַ(����뵥���ʹ���NULL)
  * @retval ��
  */
void NRF_FHSS::TxProcess(void* txbuff, void* rxbuff)
{
    /*�Ƿ�ʹ�õ���*/
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
  * @brief  ��Ƶ+��˫��(���ն�)
  * @param  rxbuff:�������ݰ���ַ
  * @param  txbuff:�������ݰ���ַ(����뵥���ʹ���NULL)
  * @retval ��
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
