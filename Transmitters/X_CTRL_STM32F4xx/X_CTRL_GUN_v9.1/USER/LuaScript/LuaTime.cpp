#include "delay.h"
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
    int time = luaL_checkinteger(L, 1);
    delay_ms(time);
    return 0;
}

void LuaReg_Time()
{
    luaScript.registerFunc("millis", Lua_millis);
    luaScript.registerFunc("micros", Lua_micros);
    luaScript.registerFunc("delay", Lua_delay);
}
