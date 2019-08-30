#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"

/*实例化显示器对象*/
TFT_SSD1283A screen(TFT_CS, TFT_RST, TFT_DC/*, TFT_SDA, TFT_SCK*/);

/*实例化GUI页面调度器*/
PageManager page(PAGE_MAX, EVENT_MAX);

/**
  * @brief  显示器初始化
  * @param  无
  * @retval 无
  */
void Init_Display()
{
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, LOW);

    screen.begin();
    screen.setRotation(1);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(false);
    screen.setTextSize(1);

    delay(200);
    digitalWrite(TFT_LED, HIGH);
}

/**
  * @brief  页面注册
  * @param  无
  * @retval 无
  */
static void Init_Pages()
{
    /*如果SD卡启动正确，使用动态主菜单*/
    if(State_SD_Enable && State_BV_Enable)
        PageRegister_MainMenuDymanic(PAGE_MainMenu);
    else
        PageRegister_MainMenu(PAGE_MainMenu);

    /*其他页面注册*/
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
    PageRegister_SetGravity(PAGE_SetGravity);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
    PageRegister_Game(PAGE_Game);
    PageRegister_FreqGraph(PAGE_FreqGraph);
    PageRegister_FileExplorer(PAGE_FileExplorer);
    PageRegister_BvPlayer(PAGE_BvPlayer);
    PageRegister_WavPlayer(PAGE_WavPlayer);

    /*跳转至主菜单*/
    page.PageChangeTo(PAGE_MainMenu);
}


/**
  * @brief  GUI初始化
  * @param  无
  * @retval 无
  */
void Init_GUI(uint8_t step)
{
    if(step == 0)
    {
        Draw_RGBBitmap_Logo(0.5, 0.5, 30);  //绘制LOGO动画
        Init_HMI();                         //初始化HMI
    }
    else
    {
        ClearDisplay();                     //清屏
        Init_StatusBar();                   //状态栏初始化
        Init_Pages();                       //页面初始化
    }
}

/**
  * @brief  在该页面延时，并保持对状态栏的刷新
  * @param  ms:延时毫秒数
  * @retval 无
  */
void PageDelay(uint32_t ms)
{
    uint32_t time = millis();
    while(millis() - time < ms)
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
