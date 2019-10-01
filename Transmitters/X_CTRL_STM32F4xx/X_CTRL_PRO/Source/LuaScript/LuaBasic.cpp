#include "TasksManage.h"
#include "LuaScript.h"

//static SemaphoreHandle_t Sem_StartLuaScript = 0;
TaskHandle_t TaskHandle_LuaScript = 0;

static const char *ScriptText = 0;

void LuaScriptStart(const char *script)
{
    ScriptText = script;
    //xSemaphoreGive(Sem_StartLuaScript);
    vTaskResume(TaskHandle_LuaScript);
}

void Task_LuaScript(void *pvParameters)
{
    luaScript.begin();
    LuaReg_Time();
    LuaReg_GPIO();
    LuaReg_ModuleCtrl();

    //Sem_StartLuaScript = xSemaphoreCreateBinary();

    if(TaskHandle_LuaScript)
        vTaskSuspend(TaskHandle_LuaScript);

    for(;;)
    {
//        if(Sem_StartLuaScript)
//        {
//            if(xSemaphoreTake(Sem_StartLuaScript, portMAX_DELAY) == pdTRUE)
//            {
        if(ScriptText)
        {
            luaScript.doString(ScriptText);
        }
        if(TaskHandle_LuaScript)
        {
            vTaskSuspend(TaskHandle_LuaScript);
        }
    }
}
