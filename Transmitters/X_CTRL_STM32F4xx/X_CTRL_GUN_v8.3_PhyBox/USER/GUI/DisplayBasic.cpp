#include "FileGroup.h"
#include "GUI_Private.h"
#include "Fonts/FreeMono12pt7b.h"
#include "PhyBox2d.h"

/*实例化显示器对象*/
TFT_SSD1283A screen(TFT_CS, TFT_RST, TFT_DC/*, TFT_SDA, TFT_SCK*/);

/*实例化GUI页面调度器*/
PageManager page(PAGE_MAX, EVENT_MAX);

Field_TypeDef Field1;
#define MassCount                   20
Particle_TypeDef MassList[MassCount];

static LightGUI::Cursor<SCREEN_CLASS> *Box[MassCount];

static void DrawBoxCallback(int16_t x,int16_t y, uint16_t color, uint16_t color_bg)
{
    screen.fillCircle(x+3,y+3,2,color);
}

static void PhyBoxTest()
{
    for(int i = 0; i < MassCount; i++)
    {
        MassList[0 + i] = Particle_StructInit(1, 50, 10, 5, 20, 1, 0, 5 * i % 120, 60 + 20 * i / 130, 0, 0);
    }
    MassList[1].Position.Y ++;
    MassList[MassCount - 2].Position.Y --;

    Field_StructInit(&Field1, 0.01);
    for(int i = 0; i < MassCount; i++)
    {
        Field_Add(&Field1, &MassList[i]);
        Box[i] = new LightGUI::Cursor<SCREEN_CLASS>(
            &screen, 0, 0, 2, 2
        );
        //Box[i]->DisplayCallbackFunction = DrawBoxCallback;
    }
    Point2D_StructInit(&Field1.Gravity, 0, 0);

    while(1)
    {
        for(int i = 0; i < 10; i++)
        {
            Field_Update(&Field1);
        }

//  screen.setCursor(0, 0);
//  screen.printf("%.2f, %.2f  ", P1.Position.X, P1.Position.Y);
//  screen.setCursor(0, 11);
//  screen.printf("%.2f, %.2f  ", P1.Velocity.X, P1.Velocity.Y);
//  screen.setCursor(0, 22);
//  screen.printf("%.2f, %.2f  ", P1.Force.X, P1.Force.Y);
        for(int i = 0; i < MassCount; i++)
        {
            Box[i]->setPosition(MassList[i].Position.X, MassList[i].Position.Y);
            //screen.fillCircle(MassList[i].Position.X, MassList[i].Position.Y, 2, screen.White);
        }
    }
}

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
    ClearDisplay();
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(true);
    screen.setTextSize(1);
    
    delay(200);
    digitalWrite(TFT_LED, HIGH);
    PhyBoxTest();
}

/**
  * @brief  页面注册
  * @param  无
  * @retval 无
  */
static void Init_Pages()
{
    /*如果SD卡启动正确，注册动态主菜单，否则使用默认菜单*/
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
    PageRegister_LuaScript(PAGE_LuaScript);

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
  * @brief  软件延时
  * @param  ms:毫秒
  * @retval 无
  */
static void SoftDealy(uint32_t ms)
{
    volatile uint32_t i = F_CPU / 1000 * ms / 5;
    while(i--);
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

/**
  * @brief  显示蓝屏崩溃报告
  * @param  无
  * @retval 无
  */
static void ShowCrashReports()
{
    screen.fillScreen(screen.Blue);
    screen.setTextColor(screen.White);
    screen.setFont(&FreeMono12pt7b);
    screen.setTextSize(2);
    screen.setCursor(0, 40);
    screen.println(":(");

    screen.setFont();
    screen.setTextSize(1);

    screen.setCursor(0, ScreenMid_H - TEXT_HEIGHT_1);
    screen.println("FATAL ERROR!");
    screen.print("Syetem will reboot...");

    screen.setCursor(0, screen.height() - TEXT_HEIGHT_1 * 6 - 2);
    screen.println("Error code:");
    screen.printf("MMFAR=0x%x\r\n", SCB->MMFAR);
    screen.printf("BFAR=0x%x\r\n", SCB->BFAR);
    screen.printf("CFSR=0x%x\r\n", SCB->CFSR);
    screen.printf("HFSR=0x%x\r\n", SCB->HFSR);
    screen.printf("DFSR=0x%x\r\n", SCB->DFSR);
}

extern "C" {
    void HardFault_Handler()
    {
        ShowCrashReports();
        SoftDealy(5000);
        NVIC_SystemReset();
    }
}
