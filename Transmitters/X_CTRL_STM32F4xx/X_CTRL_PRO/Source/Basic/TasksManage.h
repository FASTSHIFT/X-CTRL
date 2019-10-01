#ifndef __TASKSMANAGE_H
#define __TASKSMANAGE_H

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

/*Task Functions*/
void Task_Dispaly(void *pvParameters);
void Task_TransferData(void *pvParameters);
void Task_LuaScript(void *pvParameters);
void Task_WavPlayer(void *pvParameters);
void Task_PageRun(void *pvParameters);

/*Timer Functions*/
void Task_MotorRunning(TimerHandle_t xTimer);
void Task_ReadBattInfo(TimerHandle_t xTimer);

/*TaskHandle*/
extern TaskHandle_t TaskHandle_LuaScript;
extern TaskHandle_t TaskHandle_WavPlayer;

/*TimerHandle*/
extern TimerHandle_t TimerHandle_Motor;
extern TimerHandle_t TimerHandle_Charger;

#endif
