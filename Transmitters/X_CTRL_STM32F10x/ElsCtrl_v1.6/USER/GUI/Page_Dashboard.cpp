#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeSerif9pt7b.h"

void Task_UpdateDashboard()
{
	TIM_Cmd(TIM_DisplayThread, DISABLE);
	ClearPage();
	screen.setFont(&FreeSerif9pt7b);
	
	screen.setCursor(StatusBarPos + 20, 15);
	screen.setTextColor(screen.White, screen.Black);
	screen.printf("%0.1fkm/h", ElsSpeed);
	
	screen.setFont();
	screen.setCursor(StatusBarPos + 20, 25);
	screen.printf("%0.2fkm", ElsDistance);
	
	TIM_Cmd(TIM_DisplayThread, ENABLE);
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	ClearPage();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
	__IntervalExecute(Task_UpdateDashboard(), 200);
	CTRL.Left.Y = JS_L.Y;
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	TIM_Cmd(TIM_DisplayThread, ENABLE);
}

/************ 事件处理 **********/
/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
	if(btUP)
		page.PageChangeTo(PAGE_CtrlInfo);
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
void PageRegister_Dashboard(uint8_t ThisPage)
{
    /*基本*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    
    /*事件*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
