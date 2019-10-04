#include "TasksManage.h"
#include "LuaScript.h"


TaskHandle_t TaskHandle_LuaScript = 0;

static const char *ScriptText = 0;

void LuaScriptStart(const char *script)
{
    ScriptText = script;

    xTaskNotifyGive(TaskHandle_LuaScript);
}

void Task_LuaScript(void *pvParameters)
{
    luaScript.begin();
    LuaReg_Time();
    LuaReg_GPIO();
    LuaReg_ModuleCtrl();

    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ScriptText)
        {
            luaScript.doString(ScriptText);
        }
    }
}
