#include "LuaScript.h"

void lua_string_print(const char *s)
{
    luaScript.printStr(s);
}

LuaScript::LuaScript()
{
    L = 0;
    isRunning = false;
    isBegun = false;
    stringPrint = 0;
}

LuaScript::~LuaScript()
{
    end();
    lua_close(L);
    L = 0;
}

bool LuaScript::begin()
{
    if(isBegun)
        return true;

    L = luaL_newstate();
    luaL_openlibs(L);
    
    isBegun = true;
    
    return false;
}

static void breakCallback(lua_State *L, lua_Debug *ar)
{
    lua_sethook(L, NULL, 0, 0);
    if(luaScript.isRunning)
        luaL_error(L, "lua script break");
}

bool LuaScript::end()
{
    if(!isRunning)
        return false;
    
    int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT;
    lua_sethook(L, breakCallback, mask, 1);
    return true;
}

bool LuaScript::doString(const char *s)
{
    isRunning = true;
    bool error = luaL_dostring(L, s);
    isRunning = false;
    if(error)
        lua_string_print(lua_tostring(L, -1));

    return error;
}

void LuaScript::registerStrOutput(StrOutput_Callback_t func)
{
    stringPrint = func;
}

void LuaScript::registerFunc(const char *name, lua_CFunction func)
{
    lua_register(L, name, func);
}

void LuaScript::printStr(const char *s)
{
    if(stringPrint)
        stringPrint(s);
}

LuaScript luaScript;
