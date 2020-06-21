#include "NRF.h"

void NRF_FHSS::FH_Process()
{
    FH_List_Index++;
    FH_List_Index %= FH_List_Length;
    Basic->SetFreqency(FH_List[FH_List_Index]);
}

void NRF_FHSS::TxProcess(uint8_t* txbuff)
{
    FH_Process();
    Basic->TranCheck();
    Basic->Tran(txbuff);
}

void NRF_FHSS::RxProcess(uint8_t* rxbuff)
{
    if(Basic->Recv(rxbuff))
    {
        LastRxTime = millis();
        FH_Enable = true;
    }

    if(FH_Enable)
    {
        FH_Process();
    }

    if(millis() - LastRxTime >= InterruptTime * (FH_List_Length + 5))
    {
        LastRxTime = millis();
        FH_Process();
        FH_Enable = false;
    }
}

void NRF_FHSS::TxProcess(uint8_t* txbuff, uint8_t* rxbuff)
{
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
            Basic->TranSuccess_Cnt++;
            Basic->RX_Mode();
        }
        else
        {
            Basic->TX_Mode();
            Basic->Tran(txbuff);
        }
    }
}

void NRF_FHSS::RxProcess(uint8_t* rxbuff, uint8_t* txbuff)
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
            Basic->TranSuccess_Cnt++;
        }
        if(FH_Enable)
            FH_Process();
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
