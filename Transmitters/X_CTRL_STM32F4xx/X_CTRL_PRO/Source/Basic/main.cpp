#include "FileGroup.h"
#include "TasksManage.h"

static void Task_Debug(void *pvParameters)
{
    for(;;)
    {
        Serial.printf("%d\r\n", millis());
        vTaskDelay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
//    xTaskCreate(Task_Debug, (const char*)"Task_Debug",
//                 128, NULL, 1, NULL);

    xTaskCreate(Task_Dispaly, (const char*)"Task_Dispaly",
                4096, NULL, 2, NULL);

    xTaskCreate(Task_LuaScript, (const char*)"Task_LuaScript",
                10000, NULL, 0, &TaskHandle_LuaScript);

    TimerHandle_Motor = xTimerCreate("Task_MotorRunning", 10,
                              pdTRUE, 0, Task_MotorRunning);

    if( TimerHandle_Motor != NULL )
    {
        xTimerStart( TimerHandle_Motor, 0);
    }
    
    TimerHandle_Charger = xTimerCreate("Task_ReadBattInfo", 500,
                              pdTRUE, 0, Task_ReadBattInfo);

    if( TimerHandle_Charger != NULL )
    {
        xTimerStart( TimerHandle_Charger, 0);
    }

    vTaskStartScheduler();
}

void loop()
{
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);
    Delay_Init();
    ADCx_Init(ADC1);
    setup();
    for(;;)loop();
}
