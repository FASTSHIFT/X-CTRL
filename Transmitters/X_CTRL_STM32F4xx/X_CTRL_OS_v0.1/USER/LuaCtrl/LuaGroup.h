#ifndef __LUAGROUP_H
#define __LUAGROUP_H

#include "lua.hpp"
#include "HardwareSerial.h"

extern lua_State *L;
extern HardwareSerial &LuaSerial;
extern bool LuaOccupy;

void LuaReg_Time();
void LuaReg_GPIO();
void LuaReg_ModuleCtrl();
void LuaReg_GUI();

#endif
