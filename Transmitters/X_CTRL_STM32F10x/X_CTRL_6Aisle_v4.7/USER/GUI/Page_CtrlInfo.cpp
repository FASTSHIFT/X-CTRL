#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化当前页面调度器，开启优先级*/
static MillisTaskManager mtm_CtrlInfo(4, true);

/*滚动条长度:40*/
#define PBx_L 40

/*滚动条宽度:6*/
#define PBx_W 6

/*滚动条X轴Y轴起始坐标*/
#define PBY_START (screen.width()/2-(PBx_W/2))
#define PBX_START (screen.width()/2-(PBx_L/2))

/*左滚动条X轴Y轴终止坐标*/
#define PBLY_END 0
#define PBLX_END 10

/*右滚动条X轴Y轴终止坐标*/
#define PBRY_END (screen.width() - PBx_W)
#define PBRX_END (screen.width() - 10 - PBx_L)

/*实例化滚动条控件对象*/
LightGUI::ScrollBar<SCREEN_CLASS> PB_LY(&screen, PBY_START, StatusBar_POS + 4, PBx_W, PBx_L, 1, 4);
LightGUI::ScrollBar<SCREEN_CLASS> PB_RY(&screen, PBY_START, StatusBar_POS + 4, PBx_W, PBx_L, 1, 4);
LightGUI::ScrollBar<SCREEN_CLASS> PB_LX(&screen, PBX_START, screen.height() - PBx_W, PBx_L, PBx_W, 0, 4);
LightGUI::ScrollBar<SCREEN_CLASS> PB_RX(&screen, PBX_START, screen.height() - PBx_W, PBx_L, PBx_W, 0, 4);

/**
  * @brief  滚动条开启动画
  * @param  无
  * @retval 无
  */
static void ScrollBar_Open()
{
    for(double i = 0.0; i < 1.0; i += (((1.0 - i) * 0.1) + 0.001))
    {
        PB_LY.setPosition(PBY_START - i * (PBY_START - PBLY_END), PB_LY.Y);
        PB_RY.setPosition(PBY_START + i * (PBRY_END - PBY_START), PB_RY.Y);
        PB_LX.setPosition(PBX_START - i * (PBX_START - PBLX_END), PB_LX.Y);
        PB_RX.setPosition(PBX_START + i * (PBRX_END - PBX_START), PB_RX.Y);

        PB_LY.setProgress(i * 0.5);
        PB_RY.setProgress(i * 0.5);
        PB_LX.setProgress(1.0 - i * 0.5);
        PB_RX.setProgress(i * 0.5);
    }
}

/**
  * @brief  滚动条关闭动画
  * @param  无
  * @retval 无
  */
static void ScrollBar_Close()
{
    for(double i = 1.0; i > 0.0; i -= ((i * 0.1) + 0.001))
    {
        PB_LY.setPosition(PBY_START - i * (PBY_START - PBLY_END), PB_LY.Y);
        PB_RY.setPosition(PBY_START + i * (PBRY_END - PBY_START), PB_RY.Y);
        PB_LX.setPosition(PBX_START - i * (PBX_START - PBLX_END), PB_LX.Y);
        PB_RX.setPosition(PBX_START + i * (PBRX_END - PBX_START), PB_RX.Y);

        PB_LY.setProgress(i * 0.5);
        PB_RY.setProgress(i * 0.5);
    }
}

/**
  * @brief  滚动条更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawProgress()
{
    PB_LX.setProgress(fmap(CTRL.Left.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    PB_LY.setProgress(fmap(CTRL.Left.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    PB_RX.setProgress(fmap(CTRL.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    PB_RY.setProgress(fmap(CTRL.Right.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
}

/**
  * @brief  使用时间更新任务
  * @param  无
  * @retval 无
  */
static void Task_PrintUseTime()
{
    screen.setCursor(70, 20);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printf("%02d:%02d:%02d", hh, mm, ss);
}

/*static float TopSpeed = 0.0;
static void DisplayCarSSInfo()
{
    if(CarSS_Passback.Channel_1 >= 200.0)
        return;
    if(CarSS_Passback.Channel_1 > TopSpeed) 
        TopSpeed = CarSS_Passback.Channel_1;

    screen.setCursor(60, 35);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(2);
    screen.printf("%5.1f", CarSS_Passback.Channel_1);

    screen.setCursor(17, 45);
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setTextSize(1);
    screen.printf("%5.1f", TopSpeed);
}*/

/*锁定回传数据包标志位*/
bool Lock_CommonPassback = false;
/*回传数据包重新初始化标志位*/
bool Is_CommonInfoInit = false;

/**
  * @brief  显示通用回传数据
  * @param  无
  * @retval 无
  */
static void DisplayCommonInfo()
{
    /*锁定回传数据包，禁止NRF修改*/
    Lock_CommonPassback = true;
    
    if(!Is_CommonInfoInit)//是否已初始化
    {
        /*配置默认回传数据*/
        Common_Passback.X = 70;
        Common_Passback.Y = 42;
        Common_Passback.TextSize = 1;
        Common_Passback.TextColor = screen.White;
        Common_Passback.BackColor = screen.Black;
        Common_Passback.Text[0] = '\0';
        __TypeExplain(Protocol_Passback_CommonDisplay_t, NRF_RxBuff) = Common_Passback;
        Is_CommonInfoInit = true;
    }
    else
    {
        /*判断是否位于合法区域*/
        if(Common_Passback.Y > StatusBar_POS && Common_Passback.TextSize < 5)
        {
            screen.setCursor(Common_Passback.X, Common_Passback.Y);
            screen.setTextColor(Common_Passback.TextColor, Common_Passback.BackColor);
            screen.setTextSize(Common_Passback.TextSize);
            screen.print(Common_Passback.Text);
        }
    }
    
    /*解锁回传数据包*/
    Lock_CommonPassback = false;
}

/**
  * @brief  打印回传数据任务
  * @param  无
  * @retval 无
  */
void Task_PrintPassback()
{   
    if(State_PassBack && Enable_CommonPassBack)
    {
        /*if(CTRL.Info.CtrlObject == CAR_ServoSteering)
            DisplayCarSSInfo();
        else if(CTRL.Info.CtrlObject == X_COMMON)*/
        DisplayCommonInfo();
    }
}

/*初始化页面时间戳*/
static uint32_t SetupTimePoint;

/*外部函数:信号强度监视任务*/
extern void Task_SignalMonitor();

/**
  * @brief  检测信号强度任务
  * @param  无
  * @retval 无
  */
static void Task_CheckSignal()
{
    /*在进入该页面5000ms后启动信号强度监视任务*/
    if(millis() - SetupTimePoint > 5000)
        Task_SignalMonitor();
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    SetupTimePoint = millis();//记录时间戳
    PB_LX.Color_SB = screen.Yellow;
    PB_LY.Color_SB = screen.Yellow;
    PB_RX.Color_SB = screen.Yellow;
    PB_RY.Color_SB = screen.Yellow;
    ScrollBar_Open();

    uint8_t *gImage_Bitmap_x;

    if(CTRL.Info.CtrlObject == CAR_ServoSteering)
        gImage_Bitmap_x = (uint8_t*)gImage_Bitmap_RC;
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint8_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint8_t*)gImage_Bitmap_Common;

    screen.drawBitmap(15, 15, gImage_Bitmap_x, 30, 30, screen.White, screen.Black);

    mtm_CtrlInfo.TaskRegister(0, Task_DrawProgress, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_CheckSignal, 20);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, Task_PrintUseTime, 1000);
    State_RF = ON;
    Is_CommonInfoInit = false;
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    mtm_CtrlInfo.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    ScrollBar_Close();
    ClearPage();
    State_RF = OFF;
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    if(btBACK)
        page.PageChangeTo(PAGE_MainMenu);//退出到主菜单
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
    if(btEcd)
        page.PageChangeTo(PAGE_MainMenu);
}

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
static void EncoderEvent()
{
}

/**
  * @brief  控制信息页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_CtrlInfo(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
