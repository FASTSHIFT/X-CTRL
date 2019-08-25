#include "FileGroup.h"
#include "GUI_Private.h"

/*字库*/
#include "Fonts/FreeSerif9pt7b.h"

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    screen.setTextColor(screen.White);
    screen.setTextSize(0);
    screen.setFont(&FreeSerif9pt7b);
    screen.setCursor(20, 32);//10,18
    screen.print("VIFEXTech");

    screen.setFont();
    screen.setTextSize(1);
    screen.setCursor(33, 40);
    screen << "X-CTRL " << _X_CTRL_VERSION;
    screen.setCursor(13, 50);
    screen << __DATE__ << " Build";
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
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    page.PageChangeTo(PAGE_Options);
}

/**
  * @brief  设计者信息页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_About(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
