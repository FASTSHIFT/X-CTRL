#ifndef __LUA_CLASS_H
#define __LUA_CLASS_H

#include "lua.hpp"

class LuaClass {
public:
    LuaClass();
    void Init();
    void 
private:
    lua_State *L;
    Is_LuaRunning;
};

#endif
