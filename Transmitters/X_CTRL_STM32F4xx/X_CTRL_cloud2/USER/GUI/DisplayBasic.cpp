#include "FileGroup.h"
#include "GUI_Private.h"

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
	PageRegister_Ctrl(PAGE_CTRL);
	PageRegister_Main(PAGE_MAIN);
}


/**
  * @brief  GUI初始化
  * @param  无
  * @retval 无
  */
void Init_GUI()
{
    ClearDisplay();                     //清屏
    Init_Pages();                       //页面初始化
}

/**
  * @brief  GUI线程
  * @param  无
  * @retval 无
  */
void Thread_GUI()
{
    page.Running();
}
