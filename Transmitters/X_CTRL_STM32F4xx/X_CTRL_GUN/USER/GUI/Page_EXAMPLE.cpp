#include "FileGroup.h"
#include "DisplayPrivate.h"

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
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
    if(param == &btBACK)
    {
        if(event == EVENT_ButtonLongPressed)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_X(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
