#ifndef __LUASCRIPT_H
#define __LUASCRIPT_H

#include "lua.hpp"
#include "stdint.h"

/*Class*/
class LuaScript
{
    typedef void(*StrOutput_Callback_t)(const char *s);
public:
    lua_State *L;
    bool isRunning;
    bool isBegun;

    LuaScript();
    ~LuaScript();

    bool begin();
    bool end();
    bool doString(const char *s);
    void registerStrOutput(StrOutput_Callback_t func);
    void registerFunc(const char *name, lua_CFunction func);
    void printStr(const char *s);

private:
    StrOutput_Callback_t stringPrint;
};

/*Functions*/
extern LuaScript luaScript;

/*Reg*/
void LuaReg_Time();
void LuaReg_GPIO();
void LuaReg_ModuleCtrl();

void LuaScriptStart(const char *script);

#endif
