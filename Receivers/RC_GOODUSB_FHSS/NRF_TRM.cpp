#include "NRF.h"

/**
  * @brief  ���ͻ���������,�������ݵ����ջ�����(��˫��ͨ�ţ������������������źŽϲ�Ļ���)
  * @param  *txbuff:���ͻ�������ַ
  * @param  *rxbuff:���ջ�������ַ
  * @retval ��
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
  * @brief  �������ݵ����ջ�����,���ͻ���������(��˫��ͨ�ţ����ڴӻ����������źŽϲ�Ļ���)
  * @param  *rxbuff:���ջ�������ַ
  * @param  *txbuff:���ͻ�������ַ
  * @retval ��
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
