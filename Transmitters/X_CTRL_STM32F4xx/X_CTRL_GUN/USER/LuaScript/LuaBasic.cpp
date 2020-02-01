#include "FileGroup.h"
#include "LuaCallCpp.h"
#include "LuaGroup.h"
#include "DisplayPrivate.h"

/*Lua开启状态*/
bool State_LuaScript = false;

/*Lua运行标志位*/
static bool Is_LuaExit = false;

bool Lua_Begin()
{
    luaScript.close();
    luaScript.begin();
    
    LuaReg_Com();
    LuaReg_GPIO();
    LuaReg_GUI();
    LuaReg_ModuleCtrl();
    LuaReg_MouseKeyboard();
    LuaReg_Time();
    return luaScript.isBegun;
}

/**
  * @brief  自定义命令处理
  * @param  cmd:命令
  * @retval 无
  */
static bool CMD_Handler(String cmd)
{
    bool ret = true;

    if(cmd == "exit")
        Is_LuaExit = true;
    else if(cmd == "reboot")
        NVIC_SystemReset();
    else
        ret = false;

    return ret;
}

/**
  * @brief  Lua串口中断回调
  * @param  无
  * @retval 无
  */
static void Lua_USART_Callback()
{
    if(luaScript.isRunning)
    {
        char rcev = Lua_SERIAL.read();
        if(rcev == 'C')
        {
            luaScript.end();
        }
        else if(rcev == 'R')
        {
            NVIC_SystemReset();
        }

        Lua_SERIAL.flush();
    }
}

static void Lua_USART_Print(const char *s)
{
    Lua_SERIAL.print(s);
}

/**
  * @brief  Lua脚本解释器初始化
  * @param  无
  * @retval 无
  */
void Init_LuaScript()
{
    DEBUG_FUNC();
    ClearDisplay();
    TextSetDefault();
    TextMidPrint(0.5f, 0.5f, "Lua MODE");

    Lua_SERIAL.begin(115200);
    Lua_SERIAL.setTimeout(100);
    Lua_SERIAL.attachInterrupt(Lua_USART_Callback);
    Lua_SERIAL.println("\r\nInit lua script...");

    Is_LuaExit = false;

    Lua_Begin();
    
    luaScript.registerStrOutput(Lua_USART_Print);
    luaScript.doString("print(\"> Version:\",_VERSION)");

    while (!Is_LuaExit)
    {
        if(Lua_SERIAL.available())
        {
            String UserInput = Lua_SERIAL.readString();

            if(UserInput.startsWith("CMD:"))
            {
                String Cmd = UserInput.substring(4);
                if(!CMD_Handler(Cmd))
                {
                    Lua_SERIAL.println();
                    Lua_SERIAL.print(Cmd);
                    Lua_SERIAL.println(" is error cmd!");
                    Lua_SERIAL.flush();
                }
            }
            else
            {
                const char *LuaCode_buffer = UserInput.c_str();

                Lua_SERIAL.println("\r\nCode:\r\n");
                Lua_SERIAL.println(UserInput);
                Lua_SERIAL.printf("\r\n(Size: %d )\r\n", UserInput.length());
                Lua_SERIAL.print("\r\n> ");

                luaScript.doString(LuaCode_buffer);
                Lua_SERIAL.flush();
            }
        }
    }

    ClearDisplay();
    Lua_SERIAL.println("lua script exit");
}
