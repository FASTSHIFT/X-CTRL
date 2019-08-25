#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"

/*实例化显示器对象*/
TFT_S6B33B2 screen(TFT_PORT_START_Pin, TFT_RST_Pin, TFT_CS_Pin, TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin);

/*实例化GUI页面调度器*/
PageManager page(PAGE_MAX, EVENT_MAX);

/**
  * @brief  显示器初始化
  * @param  无
  * @retval 无
  */
void Init_Display()
{
    pinMode(TFT_BLK_Pin, OUTPUT);
    digitalWrite(TFT_BLK_Pin, LOW);
    
    screen.begin();
	screen.setRotation(1);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);
    
    digitalWrite(TFT_BLK_Pin, HIGH);
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
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
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
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //绘制LOGO动画
    ClearDisplay();                     //清屏
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
