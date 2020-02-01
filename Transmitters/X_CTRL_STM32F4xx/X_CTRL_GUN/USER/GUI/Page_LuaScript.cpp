#include "FileGroup.h"
#include "FileSystem.h"
#include "DisplayPrivate.h"
#include "LuaGroup.h"

/*Lua文件路径*/
String LuaFilePath;

static void ScreenPrint(const char* s)
{
    TextSetDefault();
    if(screen.getCursorY() > screen.height() - TEXT_HEIGHT_1 || screen.getCursorY() < TEXT_HEIGHT_1)
    {
        ClearDisplay();
        screen.setCursor(0, TEXT_HEIGHT_1);
    }
    screen.print(s);
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    CloseStatusBar();
    ClearPage();
    TextSetDefault();
    screen.setTextWrap(true);
    
    luaScript.registerStrOutput(ScreenPrint);
    Lua_Begin();
    
    ScreenPrint("File:");
    ScreenPrint(LuaFilePath.c_str());
    ScreenPrint("\nExecuting...");
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    luaScript.doFile(LuaFilePath.c_str());
    ScreenPrint("\nExiting...");
    PageDelay(1000);
    page.PagePop();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    ClearDisplay();
    Init_StatusBar();
}

/**
  * @brief  按键事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(param == &btBACK)
    {
        if(event == EVENT_ButtonPress)
        {
        }
        else if(event == EVENT_ButtonLongPressed)
        {
            luaScript.end();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_LuaScript(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
