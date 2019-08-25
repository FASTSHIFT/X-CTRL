#include "Arduino.h"
#include "LuaGroup.h"

static int Lua_pinMode(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 3)
    {
        lua_pushstring(L, "Error! Example: pinMode(1,1,\"OUTPUT\") -> PA1 OUTPUT Mode");
        lua_error(L);
    }

    if(!lua_isinteger(L, 1))lua_pushstring(L, "Error GPIOx !"), lua_error(L);
    if(!lua_isinteger(L, 2))lua_pushstring(L, "Error Pinx !"), lua_error(L);
    if(!lua_isstring(L, 3))lua_pushstring(L, "Error Mode !"), lua_error(L);

    int GPIOx = lua_tonumber(L, 1);
    int Pinx = lua_tonumber(L, 2);
    if(GPIOx < 1 || GPIOx > 7) lua_pushstring(L, "Error GPIOx is'A'->1 ~'G'->7!"), lua_error(L);
    if(Pinx < 0 || Pinx > 15) lua_pushstring(L, "Error Pinx is 0~15!"), lua_error(L);
    int pin = (GPIOx - 1) * 16 + Pinx;

    String Mode = String(lua_tostring(L, 3));
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
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: togglePin(1,1) ->toggle PA1 State");
        lua_error(L);
    }

    for(int i = 1; i <= 2; i++)
    {
        if (!lua_isinteger(L, i))
        {
            lua_pushstring(L, "Error number of Value!");
            lua_error(L);
        }
    }

    int GPIOx = lua_tonumber(L, 1);
    int Pinx = lua_tonumber(L, 2);

    if(GPIOx < 1 || GPIOx > 11)
    {
        lua_pushstring(L, "Error GPIOx is'A'->1 ~'K'->11!");
        lua_error(L);
    }
    if(Pinx < 0 || Pinx > 15)
    {
        lua_pushstring(L, "Error Pinx is 0~15!");
        lua_error(L);
    }

    int pin = (GPIOx - 1) * 16 + Pinx;

    togglePin(pin);
    return 0;
}

static int Lua_digitalWrite(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 3)
    {
        lua_pushstring(L, "Error! Example: digitalWrite(1,1,1) ->PA1 is 1");
        lua_error(L);
    }

    for(int i = 1; i <= 3; i++)
    {
        if (!lua_isinteger(L, i))
        {
            lua_pushstring(L, "Error number of Value!");
            lua_error(L);
        }
    }

    int GPIOx = lua_tonumber(L, 1);
    int Pinx = lua_tonumber(L, 2);
    int State = lua_tonumber(L, 3);

    if(GPIOx < 1 || GPIOx > 11)
    {
        lua_pushstring(L, "Error GPIOx is'A'->1 ~'K'->11!");
        lua_error(L);
    }
    if(Pinx < 0 || Pinx > 15)
    {
        lua_pushstring(L, "Error Pinx is 0~15!");
        lua_error(L);
    }

    int pin = (GPIOx - 1) * 16 + Pinx;

    digitalWrite(pin, State);
    return 0;
}

static int Lua_digitalRead(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: digitalRead(1,1) ->Read PA1 State");
        lua_error(L);
    }

    for(int i = 1; i <= 2; i++)
    {
        if (!lua_isinteger(L, i))
        {
            lua_pushstring(L, "Error number of Value!");
            lua_error(L);
        }
    }

    int GPIOx = lua_tonumber(L, 1);
    int Pinx = lua_tonumber(L, 2);

    if(GPIOx < 1 || GPIOx > 11)
    {
        lua_pushstring(L, "Error GPIOx is'A'->1 ~'K'->11!");
        lua_error(L);
    }
    if(Pinx < 0 || Pinx > 15)
    {
        lua_pushstring(L, "Error Pinx is 0~15!");
        lua_error(L);
    }

    int pin = (GPIOx - 1) * 16 + Pinx;

    lua_pushnumber(L, digitalRead(pin));
    return 1;
}

static int Lua_analogWrite(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 3)
    {
        lua_pushstring(L, "Error! Example: analogWrite(1,1,500) ->PA1 PWM 50%");
        lua_error(L);
    }

    for(int i = 1; i <= 3; i++)
    {
        if (!lua_isinteger(L, i))
        {
            lua_pushstring(L, "Error number of Value!");
            lua_error(L);
        }
    }

    int GPIOx = lua_tonumber(L, 1);
    int Pinx = lua_tonumber(L, 2);
    int Duty = lua_tonumber(L, 3);

    if(GPIOx < 1 || GPIOx > 7)
    {
        lua_pushstring(L, "Error GPIOx is'A'->1 ~'G'->7!");
        lua_error(L);
    }
    if(Pinx < 0 || Pinx > 15)
    {
        lua_pushstring(L, "Error Pinx is 0~15!");
        lua_error(L);
    }

    int pin = (GPIOx - 1) * 16 + Pinx;

    analogWrite(pin, Duty);
    return 0;
}

static int Lua_analogRead(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: analogRead(1,1) ->Read PA1 ADC");
        lua_error(L);
    }

    for(int i = 1; i <= 2; i++)
    {
        if (!lua_isinteger(L, i))
        {
            lua_pushstring(L, "Error number of Value!");
            lua_error(L);
        }
    }

    int GPIOx = lua_tonumber(L, 1);
    int Pinx = lua_tonumber(L, 2);

    if(GPIOx < 1 || GPIOx > 7)
    {
        lua_pushstring(L, "Error GPIOx is'A'->1 ~'G'->7!");
        lua_error(L);
    }
    if(Pinx < 0 || Pinx > 15)
    {
        lua_pushstring(L, "Error Pinx is 0~15!");
        lua_error(L);
    }

    int pin = (GPIOx - 1) * 16 + Pinx;

    lua_pushnumber(L, analogRead(pin));
    return 1;
}

void LuaReg_GPIO()
{
    lua_register(L, "pinMode", Lua_pinMode);
    lua_register(L, "togglePin", Lua_togglePin);
    lua_register(L, "digitalWrite", Lua_digitalWrite);
    lua_register(L, "digitalRead", Lua_digitalRead);
    lua_register(L, "analogWrite", Lua_analogWrite);
    lua_register(L, "analogRead", Lua_analogRead);
}
