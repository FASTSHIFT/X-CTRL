#include "Filegroup.h"
#include "GUI_Private.h"
#include "doom_nano.h"

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    doom_setup();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    doom_loop();
    if(btDOWN)
        page.PageChangeTo(PAGE_About);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
}

/************ 事件处理 **********/
/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
}

/**
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_GameDoom(uint8_t ThisPage)
{
    /*基本*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    
    /*事件*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
