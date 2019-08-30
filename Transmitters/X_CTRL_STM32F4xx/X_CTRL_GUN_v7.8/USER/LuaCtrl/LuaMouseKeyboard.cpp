#include "FileGroup.h"
#include "LuaGroup.h"

static int Lua_KeyboardPrint(lua_State *L)
{
    int nValue = lua_gettop(L);

    if(nValue != 1 || !lua_isstring(L, 1))
    {
        lua_pushstring(L, "Error input! Example: kb_print(\"hello lua!\")");
        lua_error(L);
    }

    bool status = Keyboard_Print((char*)lua_tostring(L,1));
    lua_pushboolean(L, status);
    return 1;
}

void LuaReg_MouseKeyboard()
{
    lua_register(L, "kb_print", Lua_KeyboardPrint);
}
