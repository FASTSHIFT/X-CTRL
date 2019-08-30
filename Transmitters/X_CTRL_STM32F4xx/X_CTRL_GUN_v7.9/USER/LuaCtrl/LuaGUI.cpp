#include "LuaGroup.h"
#include "GUI_Private.h"

using namespace LightGUI;

ScrollBar<SCREEN_CLASS> *Scroll;
uint8_t Scroll_CreatCnt = 0;

static int Lua_CreatScrollBar(lua_State *L)
{
    int16_t x, y, w, h, dir, l;
    int nValue = lua_gettop(L);
    if(nValue != 6)
    {
        lua_pushstring(L, "Error! Example: crtScroll(10,10,50,40,1,4)");
        lua_error(L);
    }

    x = lua_tointeger(L, 1);
    y = lua_tointeger(L, 2);
    w = lua_tointeger(L, 3);
    h = lua_tointeger(L, 4);
    dir = lua_tointeger(L, 5);
    l = lua_tointeger(L, 6);

    Scroll = new LightGUI::ScrollBar<SCREEN_CLASS>(
        &screen,
        x,
        StatusBar_POS + y,
        w,
        h,
        dir,
        l
    );
    return 0;
}

static int Lua_SetScrollBar(lua_State *L)
{
    float scroll;
    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: setScroll(0.5)");
        lua_error(L);
    }

    scroll = lua_tonumber(L, 1);
    Scroll[0].setScroll(scroll);

    return 0;
}

static int Lua_FillScreen(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1 || !lua_isstring(L, 1))
    {
        lua_pushstring(L, "Error! Example: fillscreen(\"BLUE\")");
        lua_error(L);
    }
    
    String color = String(lua_tostring(L, 1));
    color.toUpperCase();
    if(color == "BLACK")screen.fillScreen(screen.Black);
    else if(color == "WHITE")screen.fillScreen(screen.White);
    else if(color == "RED")screen.fillScreen(screen.Red);
    else if(color == "GREEN")screen.fillScreen(screen.Green);
    else if(color == "BLUE")screen.fillScreen(screen.Blue);
    else if(color == "YELLOW")screen.fillScreen(screen.Yellow);
    return 0;
}

void LuaReg_GUI()
{
    lua_register(L, "crtScroll", Lua_CreatScrollBar);
    lua_register(L, "setScroll", Lua_SetScrollBar);
    lua_register(L, "fillscreen", Lua_FillScreen);
}
