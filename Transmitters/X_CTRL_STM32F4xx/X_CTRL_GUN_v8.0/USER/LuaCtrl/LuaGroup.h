#ifndef __LUAGROUP_H
#define __LUAGROUP_H

#include "lua.hpp"

extern lua_State *L;

typedef void(*LuaOutput_Callback_t)(const char *s);

extern bool LuaOccupy;

bool Lua_Open();
bool Lua_DoString(const char *s);
void Lua_ForceToBreak();

void LuaReg_Time();
void LuaReg_GPIO();
void LuaReg_ModuleCtrl();
void LuaReg_Com();
void LuaReg_GUI();
void LuaReg_MouseKeyboard();
void LuaReg_Output(LuaOutput_Callback_t func);

#endif
