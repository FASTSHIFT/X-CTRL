#include "FileGroup.h"
#include "TasksManage.h"

#define USE_STACK_CHECK 4

#if (USE_STACK_CHECK > 0)
volatile uint32_t FreeStackSize[USE_STACK_CHECK];
TimerHandle_t TimerHandle_FreeStackMonitor; 
void Task_FreeStackMonitor(TimerHandle_t xTimer)
{
    FreeStackSize[0] = uxTaskGetFreeStackByte(TaskHandle_Display);
    FreeStackSize[1] = uxTaskGetFreeStackByte(TaskHandle_WavPlayer);
    FreeStackSize[2] = uxTaskGetFreeStackByte(TaskHandle_PageRun);
    FreeStackSize[3] = uxTaskGetFreeStackByte(TaskHandle_LuaScript);
}
#endif

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    
    /*Task Create*/
    xTaskReg(Task_Dispaly,   KByteToWord(5), 2, &TaskHandle_Display);
    //xTaskReg(Task_WavPlayer, 512,    0, &TaskHandle_WavPlayer);
    xTaskReg(Task_PageRun,   KByteToWord(2), 1, &TaskHandle_PageRun);
    xTaskReg(Task_LuaScript, KByteToWord(2), 0, &TaskHandle_LuaScript);
    //xTaskReg(Task_TransferData, KByteToWord(0.5f), 0, &TaskHandle_TransferData);
    
    /*Timer Create*/
    TimerHandle_Motor = xTimerReg(Task_MotorRunning, 10);
    xTimerStartSafe(TimerHandle_Motor);

    TimerHandle_Charger = xTimerReg(Task_ReadBattInfo, 500);
    xTimerStartSafe(TimerHandle_Charger);
    
//    TimerHandle_IMU_Claculate = xTimerReg(Task_IMU_Claculate, 500);
//    xTimerStartSafe(TimerHandle_IMU_Claculate);

#if (USE_STACK_CHECK > 0)    
    TimerHandle_FreeStackMonitor = xTimerReg(Task_FreeStackMonitor, 1000);
    xTimerStartSafe(TimerHandle_FreeStackMonitor);
#endif

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
    setup();
    for(;;)loop();
}
