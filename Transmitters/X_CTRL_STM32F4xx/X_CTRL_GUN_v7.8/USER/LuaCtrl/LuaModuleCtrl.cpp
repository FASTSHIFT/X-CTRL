#include "FileGroup.h"
#include "LuaCallCpp.h"
#include "LuaGroup.h"

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

static int Lua_BuzzTone(lua_State *L)
{
    int nValue = lua_gettop(L);
    float freq;
    uint32_t ms;
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: tone(1000, 500) -> 1000Hz 500ms");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use number set frequency");
        lua_error(L);
    }

    if (!lua_isinteger(L, 2))
    {
        lua_pushstring(L, "Error! Use intger to ctrl delay time");
        lua_error(L);
    }
    freq = lua_tonumber(L, 1);
    ms  = lua_tonumber(L, 2);
    BuzzTone(freq, ms);
    return 0;
}

static int Lua_BuzzMusic(lua_State *L)
{
    int nValue = lua_gettop(L);
    uint8_t music;
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: music(1)");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use number set music");
        lua_error(L);
    }

    music = lua_tonumber(L, 1);
    BuzzMusic(music);
    return 0;
}

void LuaReg_ModuleCtrl()
{
    lua_register(L, "motor", Lua_MotorVibrate);
    lua_register(L, "tone", Lua_BuzzTone);
    lua_register(L, "music", Lua_BuzzMusic);
}
