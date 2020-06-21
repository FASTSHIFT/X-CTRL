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

static int Lua_GetKey(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1 || !lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Example: i = key(1)");
        lua_error(L);
    }

    uint8_t key = lua_tonumber(L, 1);
    uint8_t val = 0;
    switch(key)
    {
        case 0:
            val = btUP.GetClicked();
            break;
        case 1:
            val = btDOWN.GetClicked();
            break;
        case 2:
            val = btOK.GetClicked();
            break;
        case 3:
            val = btBACK.GetClicked();
            break;
    }
    lua_pushboolean(L, val);
    return 1;
}

void LuaReg_ModuleCtrl()
{
    luaScript.registerFunc("motor", Lua_MotorVibrate);
    luaScript.registerFunc("tone", Lua_BuzzTone);
    luaScript.registerFunc("music", Lua_BuzzMusic);
    luaScript.registerFunc("key", Lua_GetKey);
}
