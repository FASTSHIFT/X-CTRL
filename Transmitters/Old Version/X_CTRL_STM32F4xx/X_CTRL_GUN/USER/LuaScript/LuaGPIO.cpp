#include "Arduino.h"
#include "LuaScript.h"

#define LUA_GET_PIN() int pin = Lua_CheckGPIO(L);if(pin < 0)return 0

static int Lua_CheckGPIO(lua_State *L)
{
    int GPIOx = luaL_checkinteger(L, 1);
    int Pinx = luaL_checkinteger(L, 2);

    if(GPIOx < 1 || GPIOx > 11)
    {
        lua_pushstring(L, "Error GPIOx is'A'->1 ~'K'->11!");
        lua_error(L);
        return -1;
    }
    if(Pinx < 0 || Pinx > 15)
    {
        lua_pushstring(L, "Error Pinx is 0~15!");
        lua_error(L);
        return -1;
    }

    return ((GPIOx - 1) * 16 + Pinx);
}

static int Lua_pinMode(lua_State *L)
{
    LUA_GET_PIN();

    String Mode = String(luaL_checkstring(L, 3));
    Mode.toUpperCase();
    if(Mode == "OUTPUT")pinMode(pin, OUTPUT);
    else if(Mode == "INPUT")pinMode(pin, INPUT);
    else if(Mode == "INPUT_PULLUP")pinMode(pin, INPUT_PULLUP);
    else if(Mode == "INPUT_PULLDOWN")pinMode(pin, INPUT_PULLDOWN);
    else if(Mode == "INPUT_ANALOG")pinMode(pin, INPUT_ANALOG);
    else if(Mode == "PWM")pinMode(pin, PWM);
    else
    {
        lua_pushstring(L, "Error Pin Mode! \r\nMode: OUTPUT, INPUT, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_ANALOG, PWM");
        lua_error(L);
    }
    return 0;
}

static int Lua_togglePin(lua_State *L)
{
    LUA_GET_PIN();
    togglePin(pin);
    return 0;
}

static int Lua_digitalWrite(lua_State *L)
{
    LUA_GET_PIN();
    digitalWrite(pin, luaL_checkinteger(L, 3));
    return 0;
}

static int Lua_digitalRead(lua_State *L)
{
    LUA_GET_PIN();
    lua_pushnumber(L, digitalRead(pin));
    return 1;
}

static int Lua_analogWrite(lua_State *L)
{
    LUA_GET_PIN();
    analogWrite(pin, luaL_checkinteger(L, 3));
    return 0;
}

static int Lua_analogRead(lua_State *L)
{
    LUA_GET_PIN();
    lua_pushnumber(L, analogRead(pin));
    return 1;
}

void LuaReg_GPIO()
{
    luaScript.registerFunc("pinMode", Lua_pinMode);
    luaScript.registerFunc("togglePin", Lua_togglePin);
    luaScript.registerFunc("digitalWrite", Lua_digitalWrite);
    luaScript.registerFunc("digitalRead", Lua_digitalRead);
    luaScript.registerFunc("analogWrite", Lua_analogWrite);
    luaScript.registerFunc("analogRead", Lua_analogRead);
}
