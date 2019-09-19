#include "Arduino.h"
#include "LuaScript.h"

static int Lua_millis(lua_State *L)
{
    lua_pushnumber(L, millis());
    return 1;
}

static int Lua_micros(lua_State *L)
{
    lua_pushnumber(L, micros());
    return 1;
}

static int Lua_delay(lua_State *L)
{
    int nValue = lua_gettop(L);
    unsigned long DelayTime;
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: delay(1000) -> delay 1000ms");
        lua_error(L);
    }
    if (!lua_isinteger(L, 1))
    {
        lua_pushstring(L, "Error! Use Integer to delay");
        lua_error(L);
    }
    DelayTime = lua_tonumber(L, 1);
    delay(DelayTime);
    return 0;
}

void LuaReg_Time()
{
    luaScript.registerFunc("millis", Lua_millis);
    luaScript.registerFunc("micros", Lua_micros);
    luaScript.registerFunc("delay", Lua_delay);
}
