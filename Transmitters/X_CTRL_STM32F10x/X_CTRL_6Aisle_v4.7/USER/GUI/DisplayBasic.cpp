#include "FileGroup.h"
#include "GUI_Private.h"

/*实例化显示器对象*/
SCREEN_CLASS screen(-1);

/*实例化GUI页面调度器*/
PageManager page(PAGE_MAX, EVENT_MAX);

/*屏幕更新任务*/
void Task_ScreenDisplay()
{
	screen.display();
}

/**
  * @brief  显示器初始化
  * @param  无
  * @retval 无
  */
void Init_Display()
{
    __LoopExecute(screen.begin(SSD1306_SWITCHCAPVCC), 100);
    screen.setTextSize(1);
    screen.setTextColor(screen.White);
    screen.display();
}

/**
  * @brief  页面注册
  * @param  无
  * @retval 无
  */
static void Init_Pages()
{
    PageRegister_MainMenu(PAGE_MainMenu);
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
}


/**
  * @brief  GUI初始化
  * @param  无
  * @retval 无
  */
void Init_GUI()
{
    screen.clearDisplay();              //清屏
    Init_StatusBar();                   //状态栏初始化
    Init_Pages();                       //页面初始化
}

/**
  * @brief  在该页面延时，并保持对状态栏的刷新
  * @param  ms:延时毫秒数
  * @retval 无
  */
void PageDelay(uint32_t ms)
{
    uint32_t Stop_TimePoint = millis() + ms;
    while(millis() < Stop_TimePoint)
        Thread_StatusBar();
}

/**
  * @brief  GUI线程
  * @param  无
  * @retval 无
  */
void Thread_GUI()
{
    Thread_StatusBar();
    page.Running();
}
