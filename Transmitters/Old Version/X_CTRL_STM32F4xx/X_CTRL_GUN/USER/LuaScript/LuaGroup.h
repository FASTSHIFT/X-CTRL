#ifndef __LUAGROUP_H
#define __LUAGROUP_H

#include "lua.hpp"
#include "LuaScript.h"
#include "DisplayPrivate.h"

bool Lua_Begin();

void LuaReg_Time();
void LuaReg_GPIO();
void LuaReg_ModuleCtrl();
void LuaReg_Com();
void LuaReg_GUI();
void LuaReg_MouseKeyboard();

#endif
