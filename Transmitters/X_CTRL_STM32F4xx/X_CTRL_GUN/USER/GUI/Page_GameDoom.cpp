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
    if(btBACK)
        page.PagePop();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
}

/**
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_GameDoom(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
