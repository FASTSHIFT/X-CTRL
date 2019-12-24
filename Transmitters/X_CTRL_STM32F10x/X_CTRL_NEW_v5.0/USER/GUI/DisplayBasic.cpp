#include "FileGroup.h"
#include "GUI_Private.h"

/*实例化显示器对象*/
SCREEN_CLASS screen(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

/*实例化GUI页面调度器*/
PageManager page(PAGE_MAX, EVENT_MAX);

/**
  * @brief  显示器初始化
  * @param  无
  * @retval 无
  */
void Init_Display()
{
    screen.initR(INITR_MINI128x64);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);
}

/**
  * @brief  页面注册
  * @param  无
  * @retval 无
  */
static void Init_Pages()
{
    PAGE_REG(MainMenu);
    PAGE_REG(CtrlInfo);
    PAGE_REG(Options);
    PAGE_REG(SetJoystick);
    PAGE_REG(SetBluetooth);
    PAGE_REG(SetGravity);
    PAGE_REG(About);
    PAGE_REG(Handshake);
    PAGE_REG(FreqGraph);
}


/**
  * @brief  GUI初始化
  * @param  无
  * @retval 无
  */
void Init_GUI()
{
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //绘制LOGO动画
    Init_HMI();                         //初始化HMI
    screen.clear();                     //清屏
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
