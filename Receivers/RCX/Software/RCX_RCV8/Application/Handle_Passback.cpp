#include "ComPrivate.h"
#include "FileGroup.h"

static int16_t BattVoltage;
static int16_t BattLevel;

#define DIV_RES_1 20000.0f
#define DIV_RES_2 20000.0f

#define BATT_MIN_VOLTAGE 3500 /* mV */
#define BATT_MAX_VOLTAGE 4200

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
    if (RCX::Handshake::GetPassBack_Enable())
    {
        /*Vch1 = VREFINT* (( ADch1*(VREF/4096))/(ADre*(VREF/4096))*/
        /*VREFINT = 1.2V VREF = 3.3V*/
        //BattVoltage = 1.2f * (float(analogRead_DMA(BAT_DET_Pin)*(3.3f/4096))/float(ADC_DMA_GetValue(ADC_Channel_Vrefint)*(3.3f/4096)));

        BattVoltage = analogRead(BAT_DET_Pin) / 4.095f * 3.3f * ((DIV_RES_1 + DIV_RES_2) / DIV_RES_2);
        BattLevel = map(BattVoltage, BATT_MIN_VOLTAGE, BATT_MAX_VOLTAGE, 0, RCX_CHANNEL_DATA_MAX);
        BattLevel = constrain(BattLevel, 0, RCX_CHANNEL_DATA_MAX);
//        Serial.printf("ADC: %d, BattVoltage: %d, BattLevel: %d\r\n", analogRead(BAT_DET_Pin), BattVoltage, BattLevel);
    }
}
