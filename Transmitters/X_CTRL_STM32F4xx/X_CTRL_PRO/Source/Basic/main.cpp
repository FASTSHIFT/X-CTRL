#include "FileGroup.h"
#include "TasksManage.h"

void setup()
{
    Serial.begin(115200);
    
    /*Task Create*/
    xTaskReg(Task_Dispaly,   7 * 1024, 2, NULL);
    //xTaskReg(Task_WavPlayer, 512,      0, &TaskHandle_WavPlayer);
    xTaskReg(Task_PageRun,   1 * 1024, 1, NULL);
    xTaskReg(Task_LuaScript, 4 * 1024, 0, &TaskHandle_LuaScript);

    /*Timer Create*/
    TimerHandle_Motor = xTimerReg(Task_MotorRunning, 10);
    xTimerStartSafe(TimerHandle_Motor);

    TimerHandle_Charger = xTimerReg(Task_ReadBattInfo, 500);
    xTimerStartSafe(TimerHandle_Charger);

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Delay_Init();
    ADCx_Init(ADC1);
    setup();
    for(;;)loop();
}
