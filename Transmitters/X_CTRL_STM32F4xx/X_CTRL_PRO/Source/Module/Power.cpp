#include "FileGroup.h"
#include "TasksManage.h"
#include "IP5108.h"
#include "Module.h"

TimerHandle_t TimerHandle_Charger;

static IP5108 charger;

float BattCurret, BattVoltage, BattVoltageOc;

void Task_ReadBattInfo(TimerHandle_t xTimer)
{
    __ExecuteOnce(charger.begin());
    BattCurret = charger.getBattCurrent();
    BattVoltage = charger.getBattVoltage();
    BattVoltageOc = charger.getBattOcVoltage();
    //Serial.printf("Batt U = %0.2fmV, I = %0.2fmA, ", charger.getBattVoltage(), charger.getBattCurrent());
    //Serial.printf("Oc U = %0.2fmV\r\n", charger.getBattOcVoltage());
}

void Power_Shutdown()
{
    pinMode(CHG_KEY_Pin, OUTPUT_OPEN_DRAIN);
    
    digitalWrite(CHG_KEY_Pin, HIGH);   
    vTaskDelay(50);
    digitalWrite(CHG_KEY_Pin, LOW);
    vTaskDelay(50);
    digitalWrite(CHG_KEY_Pin, HIGH);
    
    vTaskDelay(50);
    digitalWrite(CHG_KEY_Pin, LOW);
    vTaskDelay(50);
    digitalWrite(CHG_KEY_Pin, HIGH);
    
    pinMode(CHG_KEY_Pin, INPUT);
}
