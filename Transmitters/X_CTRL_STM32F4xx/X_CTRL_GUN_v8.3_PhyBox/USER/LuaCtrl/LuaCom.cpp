#include "FileGroup.h"
#include "LuaGroup.h"
#include "ComPrivate.h"

static int Lua_SetRFState(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: setRF(1) -> Turn on RF");
        lua_error(L);
    }
    if (!lua_isinteger(L, 1))
    {
        lua_pushstring(L, "Error! Use integer set RF state");
        lua_error(L);
    }
    State_RF = lua_tointeger(L, 1);
    State_RF ? Lua_SERIAL.println("RF Enalbe") : Lua_SERIAL.println("RF Disable");
    return 0;
}


static uint8_t ItemSelect_MAX = 0;
static int Lua_Handshake(lua_State *L)
{
    State_RF = OFF;//遥控关闭
    ItemSelect_MAX = 0;

    int nValue = lua_gettop(L);
    if(nValue != 1 || !lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Example: handshake(2000) -> run handshake process 2000ms");
        lua_error(L);
    }

    Lua_SERIAL.println("Handshake preparing...");
    /*主机准备握手*/
    HandshakeRun(HandshakeState_Prepare);

    uint32_t StopTime = millis() + lua_tonumber(L, 1);
    uint8_t ItemSelect_MAX_Last = 0;
    while(millis() < StopTime)
    {
        /*获取从机列表数量*/
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);

        if(ItemSelect_MAX != ItemSelect_MAX_Last)
        {
            Lua_SERIAL.printf(
                "Find-> ID:0x%x %s\r\n",
                HandshakePack_Slave[ItemSelect_MAX_Last].ID,
                HandshakePack_Slave[ItemSelect_MAX_Last].Description
            );

            ItemSelect_MAX_Last = ItemSelect_MAX;
        }
    }

    /*显示搜索结果*/
    if(ItemSelect_MAX > 0)
    {
        Lua_SERIAL.println("Search Done!");
    }
    else
    {
        Lua_SERIAL.println("Not Found!");
    }

    lua_pushnumber(L, ItemSelect_MAX);

    return 1;
}

static int Lua_ConnectSlave(lua_State *L)
{
    if(ItemSelect_MAX == 0)
    {
        lua_pushstring(L, "No slave has found! please retry handshake");
        lua_error(L);
    }

    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: connect(1) -> connect to slave(1)");
        lua_error(L);
    }
    if (!lua_isinteger(L, 1))
    {
        lua_pushstring(L, "Error! Use integer set slave");
        lua_error(L);
    }

    uint8_t ItemSelect = lua_tonumber(L, 1);
    if(ItemSelect >= ItemSelect_MAX)
    {
        lua_pushfstring(L, "slave num max is %d!", ItemSelect_MAX - 1);
        lua_error(L);
    }


    /*尝试连接从机*/
    Lua_SERIAL.printf("connecting to %s...\r\n", HandshakePack_Slave[ItemSelect].Description);

    /*超时设置*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*等待从机响应握手信号*/
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        /*2500ms超时*/
        if(millis() - timeout > 2500)
        {
            Lua_SERIAL.println("timeout");
            IsTimeout = true;
            break;
        }
    }

    /*握手收尾设置，跳转至约定好的握手频率以及地址*/
    HandshakeRun(HandshakeState_Connected);

    /*对应从机类型*/
    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;

    /*如果未超时表示握手成功*/
    if(!IsTimeout)
    {
        Lua_SERIAL.println("connecting successful");
    }
    
    lua_pushboolean(L, !IsTimeout);
    return 1;
}

static int Lua_GetSlaveInfo(lua_State *L)
{
    uint8_t ItemSelect = lua_tonumber(L, 1);
    if(ItemSelect >= ItemSelect_MAX)
    {
        lua_pushstring(L, "Error slave require");
        lua_error(L);
    }
    lua_pushinteger(L, HandshakePack_Slave[ItemSelect].ID);
    lua_pushstring(L, HandshakePack_Slave[ItemSelect].Description);
    lua_pushinteger(L, HandshakePack_Slave[ItemSelect].CtrlType);
    return 3;
}

static int Lua_SetChannle(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: setCh(0,1000) -> set channle0 to 1000");
        lua_error(L);
    }

    if (!(lua_isnumber(L, 1) && lua_isnumber(L, 2)))
    {
        lua_pushstring(L, "Error! Use number set channle num and value");
        lua_error(L);
    }

    SetJoystickConnectEnable(false);
    uint8_t channle = lua_tointeger(L, 1);
    float value = (float)lua_tonumber(L, 2);

    switch(channle)
    {
    case 0:
        CTRL.Left.X = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 1:
        CTRL.Left.Y = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 2:
        CTRL.Right.X = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 3:
        CTRL.Right.Y = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    default:
        lua_pushstring(L, "Error Channle num");
        lua_error(L);
        break;
    }

    return 0;
}

static int Lua_GetChannle(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 1)
    {
        lua_pushstring(L, "Error! Example: getCh(0) -> get channle0 value");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use number set channle num");
        lua_error(L);
    }

    uint8_t channle = lua_tointeger(L, 1);
    float value = 0.0;

    switch(channle)
    {
    case 0:
        value = JS_L.X;
        break;
    case 1:
        value = JS_L.Y;
        break;
    case 2:
        value = JS_R.X;
        break;
    case 3:
        value = JS_R.Y;
        break;
    default:
        lua_pushstring(L, "Error channle num");
        lua_error(L);
        break;
    }

    lua_pushnumber(L, value);

    return 1;
}

static int Lua_SetButton(lua_State *L)
{
    int nValue = lua_gettop(L);
    if(nValue != 2)
    {
        lua_pushstring(L, "Error! Example: setBt(0,1) -> press button 0");
        lua_error(L);
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushstring(L, "Error! Use number set button num");
        lua_error(L);
    }

    if (!lua_isnumber(L, 2))
    {
        lua_pushstring(L, "Error! Use number set button value");
        lua_error(L);
    }

    uint8_t bt = constrain(lua_tointeger(L, 1), 0, 7);
    bool val = lua_tointeger(L, 2);

    if(val)
    {
        CTRL.Key |= 1 << bt;
    }
    else
    {
        CTRL.Key &= ~(1 << bt);
    }

    return 0;
}

void LuaReg_Com()
{
    luaScript.registerFunc("setRF", Lua_SetRFState);
    luaScript.registerFunc("handshake", Lua_Handshake);
    luaScript.registerFunc("connect", Lua_ConnectSlave);
    luaScript.registerFunc("getSlave", Lua_GetSlaveInfo);
    luaScript.registerFunc("setCh", Lua_SetChannle);
    luaScript.registerFunc("setBt", Lua_SetButton);
    luaScript.registerFunc("getCh", Lua_GetChannle);
}
