#include "LuaGroup.h"
#include "DisplayPrivate.h"

using namespace LightGUI;

#define WIDGET_MAX 50

static uint16_t GetColor(const char *strColor)
{
    uint16_t nColor = screen.Black;
    String color = String(strColor);
    color.toUpperCase();

    if(color == "WHITE")
        nColor = screen.White;
    else if(color == "RED")
        nColor = screen.Red;
    else if(color == "GREEN")
        nColor = screen.Green;
    else if(color == "BLUE")
        nColor = screen.Blue;
    else if(color == "YELLOW")
        nColor = screen.Yellow;
    
    return nColor;
}

/************************ Scroll *******************************/
ScrollBar<SCREEN_CLASS> *Scroll[WIDGET_MAX];
uint8_t ScrollCnt = 0;

static int Lua_CreatScrollBar(lua_State *L)
{
    int16_t x, y, w, h, dir, l;
    uint16_t color_fm, color_sb;
    int nValue = lua_gettop(L);
    if(nValue != 8)
    {
        lua_pushstring(L, "Error! Example: sroll = crtScroll(10,10,50,40,1,4,\"WHITE\",\"WHITE\")");
        lua_error(L);
    }

    x = lua_tointeger(L, 1);
    y = lua_tointeger(L, 2);
    w = lua_tointeger(L, 3);
    h = lua_tointeger(L, 4);
    dir = lua_tointeger(L, 5);
    l = lua_tointeger(L, 6);
    color_fm = GetColor(lua_tostring(L, 7));
    color_sb = GetColor(lua_tostring(L, 8));

    if(ScrollCnt < WIDGET_MAX)
    {
        Scroll[ScrollCnt] = new ScrollBar<SCREEN_CLASS>(
            &screen, x, y, w, h,
            dir, l
        );
        Scroll[ScrollCnt]->Color_FM = color_fm;
        Scroll[ScrollCnt]->Color_SB = color_sb;
        ScrollCnt++;
    }

    lua_pushinteger(L, ScrollCnt);
    return 1;
}

static int Lua_SetScrollBar(lua_State *L)
{
    uint16_t id;
    float prg;
    if(lua_gettop(L) != 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2))
    {
        lua_pushstring(L, "Error! Example: setScroll(1,0.5)");
        lua_error(L);
    }

    id = lua_tonumber(L, 1) - 1;
    prg = lua_tonumber(L, 2);
    if(id < ScrollCnt)
    {
        Scroll[id]->setScroll(prg);
    }

    return 0;
}

/************************ Box *******************************/
Cursor<SCREEN_CLASS> *Box[WIDGET_MAX];
uint8_t BoxCnt = 0;

static int Lua_CreatBox(lua_State *L)
{
    float x, y;
    int16_t w, h;
    uint16_t color;
    int nValue = lua_gettop(L);
    if(nValue != 5)
    {
        lua_pushstring(L, "Error! Example: box = crtBox(0.5,0.5,50,40,\"WHITE\")");
        lua_error(L);
    }

    x = lua_tonumber(L, 1);
    y = lua_tonumber(L, 2);
    w = lua_tointeger(L, 3);
    h = lua_tointeger(L, 4);
    color = GetColor(lua_tostring(L, 5));

    if(BoxCnt < WIDGET_MAX)
    {
        Box[BoxCnt] = new Cursor<SCREEN_CLASS>(
            &screen, x * screen.width(), y * screen.height(), w, h
        );
        Box[BoxCnt]->Color_CS = color;
        BoxCnt++;
    }

    lua_pushinteger(L, BoxCnt);
    return 1;
}

static int Lua_SetBox(lua_State *L)
{
    uint16_t id;
    float x, y;
    int nValue = lua_gettop(L);
    if(nValue != 3)
    {
        lua_pushstring(L, "Error! Example: setBox(0,0.5,0.5)");
        lua_error(L);
    }

    id = lua_tointeger(L, 1) - 1;
    x = lua_tonumber(L, 2);
    y = lua_tonumber(L, 3);
    
    if(id < BoxCnt)
    {
        Box[id]->setPosition_f(x, y);
    }
    return 0;
}

/************************ Basic *******************************/
static int Lua_FillScreen(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1 || !lua_isstring(L, 1))
    {
        lua_pushstring(L, "Error! Example: fillscreen(\"BLUE\")");
        lua_error(L);
    }

    screen.fillScreen(GetColor(lua_tostring(L, 1)));
    return 0;
}

void LuaReg_GUI()
{
    luaScript.registerFunc("crtScroll", Lua_CreatScrollBar);
    luaScript.registerFunc("setScroll", Lua_SetScrollBar);
    
    luaScript.registerFunc("crtBox", Lua_CreatBox);
    luaScript.registerFunc("setBox", Lua_SetBox);
    
    luaScript.registerFunc("fillScreen", Lua_FillScreen);
}
