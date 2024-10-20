#include "FileGroup.h"
#include "ComPrivate.h"

static int16_t BattVoltage;
static int16_t BattLevel;

void Com_PassbackInit()
{
    RCX::ChannelSetAttachEnable(true);
    RCX::ChannelSetAttach(0, &BattVoltage);
    RCX::ChannelSetAttach(1, &BattLevel);
    RCX::SetTxObjectType(RCX_TYPE);
    RCX::SetTxObjectID(RCX::Handshake::GetSlaveID());
}

void Task_ComPassback(MTM::Task* task)
{
    if(RCX::Handshake::GetPassBack_Enable())
    {
        /*Vch1 = VREFINT* (( ADch1*(VREF/4096))/(ADre*(VREF/4096))*/
        /*VREFINT = 1.2V VREF = 3.3V*/
        //BattVoltage = 1.2f * (float(analogRead_DMA(BAT_DET_Pin)*(3.3f/4096))/float(ADC_DMA_GetValue(ADC_Channel_Vrefint)*(3.3f/4096)));
        
        BattVoltage = analogRead_DMA(BAT_DET_Pin) / 4095.0f * 3.3f * 2 * 1000;
        BattLevel = map(BattVoltage, 2800, 4200, 0, RCX_CHANNEL_DATA_MAX);
    }
}
