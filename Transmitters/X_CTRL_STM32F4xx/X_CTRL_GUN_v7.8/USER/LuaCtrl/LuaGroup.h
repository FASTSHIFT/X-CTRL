#ifndef __LUAGROUP_H
#define __LUAGROUP_H

#include "lua.hpp"
#include "HardwareSerial.h"

typedef void(*LuaOutput_Callback_t)(const char *s);

extern lua_State *L;
extern HardwareSerial &LuaSerial;
extern bool LuaOccupy;

void Lua_ForceToBreak();
void LuaOutputReg(LuaOutput_Callback_t func);

void LuaReg_Time();
void LuaReg_GPIO();
void LuaReg_ModuleCtrl();
void LuaReg_Com();
void LuaReg_GUI();
void LuaReg_MouseKeyboard();

#endif
