#include "Basic/FileGroup.h"
#include "IP5108/IP5108.h"
#include "BSP.h"

static IP5108 charger;

void Power_GetInfo(float* battCurrent,float* battVoltage,float* battVoltageOc)
{
    if(battCurrent)
        *battCurrent = charger.current;
    
    if(battVoltage)
        *battVoltage = charger.voltage;
    
    if(battVoltageOc)
        *battVoltageOc = charger.voltageOc;
}

float Power_GetBattUsage()
{
    float batUsage = __Map(charger.voltageOc, 2600, 4200, 0.0f, 100.0f);
    if(batUsage > 100)
        batUsage = 100;
    return batUsage;
}

void Power_Init()
{
    DEBUG_FUNC_LOG();
    charger.begin();
    pinMode(CHG_KEY_Pin, INPUT);
    pinMode(LED_Pin, OUTPUT);
    Power_SetLedState(true);
}

void Power_Update()
{
    charger.update();
//    Serial.printf("Batt U = %0.2fmV, I = %0.2fmA, ", charger.voltage, charger.current);
//    Serial.printf("Oc U = %0.2fmV\r\n", charger.voltageOc);
}

void Power_Shutdown()
{
    Brightness_SetGradual(0, 200);
    pinMode(CHG_KEY_Pin, OUTPUT_OPEN_DRAIN);
    digitalWrite(CHG_KEY_Pin, LOW);
}

void Power_SetLedState(bool state)
{
    digitalWrite(LED_Pin, state);
}
