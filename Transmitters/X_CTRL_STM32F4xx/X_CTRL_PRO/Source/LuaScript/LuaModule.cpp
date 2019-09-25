#include "FileGroup.h"
#include "LuaScript.h"

static int Lua_MotorVibrate(lua_State *L)
{
    int nValue = lua_gettop(L);
    float pwm;
    uint32_t ms;
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: motor(1.0, 50) 1.0 -> PWM 100%, 50 -> 50ms");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use float to ctrl motor power");
        lua_error(L);
    }

    if (!lua_isinteger(L, 2))
    {
        lua_pushstring(L, "Error! Use intger to ctrl motor delay time");
        lua_error(L);
    }
    pwm = lua_tonumber(L, 1);
    ms  = lua_tonumber(L, 2);
    MotorVibrate(pwm, ms);
    return 0;
}

void LuaReg_ModuleCtrl()
{
    luaScript.registerFunc("motor", Lua_MotorVibrate);
}
