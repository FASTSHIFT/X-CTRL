#include "FileGroup.h"
#include "TasksManage.h"

void setup()
{
    Serial.begin(115200);
//    /*Task Create*/
//    xTaskCreate(Task_Dispaly, (const char*)"Task_Dispaly",
//                5000, NULL, 2, NULL);
    
    xTaskCreate(Task_WavPlayer, (const char*)"TaskHandle_WavPlayer",
                 1000, NULL, 1, &TaskHandle_WavPlayer);

//    xTaskCreate(Task_LuaScript, (const char*)"Task_LuaScript",
//                4 * 1024, NULL, 1, &TaskHandle_LuaScript);

    /*Timer Create*/
    TimerHandle_Motor = xTimerCreate("Task_MotorRunning", 10,
                              pdTRUE, 0, Task_MotorRunning);

    if( TimerHandle_Motor != NULL )
    {
        xTimerStart(TimerHandle_Motor, 0);
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
