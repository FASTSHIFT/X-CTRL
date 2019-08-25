#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化当前页面调度器，开启优先级*/
static MillisTaskManager mtm_CtrlInfo(4, true);

/*滚动条宽度*/
#define PBx_Width (screen.width()*0.7)

/*滚动条高度*/
#define PBx_Height 10

/*滚动条X轴Y轴起始坐标*/
#define PBY_START (StatusBar_POS+50)
#define PBX_START ((screen.width()-PBx_Width)/2)

/*实例化滚动条控件对象*/
LightGUI::ScrollBar<SCREEN_CLASS> BarThr(&screen, PBX_START, PBY_START, PBx_Width, PBx_Height, 0, 8);
LightGUI::ScrollBar<SCREEN_CLASS> BarStr(&screen, PBX_START, PBY_START + PBx_Height + 2, PBx_Width, PBx_Height, 0, 8);

LightGUI::ProgressBar<SCREEN_CLASS> BarLimit_R(&screen, PBX_START, PBY_START + PBx_Height * 2 + 4, PBx_Width, PBx_Height, 0);
LightGUI::ProgressBar<SCREEN_CLASS> BarLimit_L(&screen, PBX_START, PBY_START + PBx_Height * 3 + 4, PBx_Width, PBx_Height, 0);


/**
  * @brief  滚动条更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawProgress()
{
    BarThr.setScroll(fmap(CTRL.Left.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));
    BarStr.setScroll(fmap(CTRL.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.01));

    BarLimit_L.setProgress(CTRL.KnobLimit.L / (float)CtrlOutput_MaxValue);
    BarLimit_R.setProgress(CTRL.KnobLimit.R / (float)CtrlOutput_MaxValue);
}

/**
  * @brief  使用时间更新任务
  * @param  无
  * @retval 无
  */
static void Task_PrintUseTime()
{
    screen.setCursor(70, StatusBar_POS + 8);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printfX("%02d:%02d:%02d", hh, mm, ss);
}

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
    ClearPage();
    SetupTimePoint = millis();//记录时间戳
    BarThr.Color_SB = screen.Yellow;
    BarStr.Color_SB = screen.Yellow;
    BarLimit_L.Color_PB = screen.White;
    BarLimit_R.Color_PB = screen.White;

    uint16_t *gImage_Bitmap_x;

    if(CTRL.Info.CtrlObject == CAR_ServoSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_RC;
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_Common;

    screen.drawRGBBitmap(15, StatusBar_POS + 3, gImage_Bitmap_x, 30, 30);

    mtm_CtrlInfo.TaskRegister(0, Task_DrawProgress, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_CheckSignal, 20);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, Task_PrintUseTime, 1000);
    
    BarLimit_R.display();
    BarLimit_L.display();

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
    ClearPage();
    State_RF = OFF;
    XFS_Speak("控制界面关闭");
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
    if(btBACK)
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
