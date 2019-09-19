#ifndef __TASKSMANAGE_H
#define __TASKSMANAGE_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/*Task Functions*/
void Task_Dispaly(void *pvParameters);
void Task_TransferData(void *pvParameters);
void Task_LuaScript(void *pvParameters);

/*TaskHandle*/
extern TaskHandle_t TaskHandle_LuaScript;

#endif
