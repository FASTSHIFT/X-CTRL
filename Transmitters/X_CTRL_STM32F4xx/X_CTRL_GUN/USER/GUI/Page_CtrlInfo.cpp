#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"
#include "IMU_Private.h"

/*实例化当前页面调度器，开启优先级*/
static MillisTaskManager mtm_CtrlInfo(4, true);

typedef struct{
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
}Area_t;

static const Area_t PB_AREA = {
    5, StatusBar_Height + Page_Height / 2 - 10,
    screen.width() - 10, Page_Height / 2 + 5
};

#define ELEMENT_WIDTH  (PB_AREA.w/2)
#define ELEMENT_HEIGHT (PB_AREA.h/4)
#define ELEMENT_X(i)   (i < 4 ? PB_AREA.x : (PB_AREA.x + ELEMENT_WIDTH))
#define ELEMENT_Y(i)   (PB_AREA.y + 5 + (i % 4) * ELEMENT_HEIGHT)


/*实例化滚动条控件对象*/
static LightGUI::ScrollBar<SCREEN_CLASS>* BarChannel_Grp[8];


/**
  * @brief  滚动条更新任务
  * @param  无
  * @retval 无
  */
static void Task_BarChannelUpdate()
{
    if(!BarChannel_Grp[0])
        return;
    
    for(int i = 0; i < __Sizeof(BarChannel_Grp); i++)
    {
        int16_t chVal = RCX::ChannelRead(i);
        float prg = __Map(chVal, -RCX_ChannelData_Max, RCX_ChannelData_Max, 0.0f, 1.0f);
        BarChannel_Grp[i]->setProgress(prg);
    }
}

/**
  * @brief  使用时间更新任务
  * @param  无
  * @retval 无
  */
static void Task_PrintUseTime()
{
    screen.setCursor(TextMid(0.8, 10), StatusBar_Height + 9);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printfX("%02d:%02d:%02d", hh, mm, ss);
}

/**
  * @brief  显示通用回传数据
  * @param  无
  * @retval 无
  */
static void DisplayCommonInfo()
{
    
}

/**
  * @brief  打印回传数据任务
  * @param  无
  * @retval 无
  */
void Task_PrintPassback()
{
    if(CTRL.State.PassBack)
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

static void DrawCtrlObjImg()
{
    uint16_t *gImage_Bitmap_x;

    uint8_t objType = RCX::GetObjectType();
    if(objType == RCX::CAR_ServoSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_RC;
    else if(objType == RCX::CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_Common;

    screen.drawRGBBitmap(15, StatusBar_Height + 3, gImage_Bitmap_x, 30, 30);
}

static void DrawBarChannel()
{
    TextSetDefault();
    for(int i = 0; i < __Sizeof(BarChannel_Grp); i++)
    {
        screen.setCursor(ELEMENT_X(i) + 2, ELEMENT_Y(i));
        screen.printf("CH%d", i);
        if(!BarChannel_Grp[i])
        {
            BarChannel_Grp[i] = new LightGUI::ScrollBar<SCREEN_CLASS>(
                    &screen,
                    ELEMENT_X(i) + 2 + TEXT_WIDTH_1 * 3,
                    ELEMENT_Y(i),
                    ELEMENT_WIDTH - 4 - TEXT_WIDTH_1 * 3,
                    TEXT_HEIGHT_1,
                    0,
                    2
                );
            BarChannel_Grp[i]->Color_SB = screen.Yellow;
        }
        BarChannel_Grp[i]->display();
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    nrf.SetRF_Enable(true);
    RCX::ChannelReset();
    ClearPage();
    SetupTimePoint = millis();//记录时间戳

    DrawCtrlObjImg();
    
    screen.drawRoundRect(PB_AREA.x, PB_AREA.y, PB_AREA.w, PB_AREA.h, 5, screen.Blue);
//    TextSetDefault();
//    TextMidPrint(0.5f, ((PB_AREA.y) / (float)screen.height()), " CHANNELS ");
    
    DrawBarChannel();

    mtm_CtrlInfo.TaskRegister(0, Task_BarChannelUpdate, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_CheckSignal, 20);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, Task_PrintUseTime, 1000);

    Com_SetRFEnable(true);
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
    Com_SetRFEnable(false);
    nrf.SetRF_Enable(false);
    XFS_Speak("控制界面关闭");
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonLongPressed)
    {
        if(param == &btBACK)
        {
            page.PageStackClear();
            page.PagePush(PAGE_MainMenu);
        }
    }
    
    if(event == EVENT_ButtonDoubleClick)
    {
        if(param == &btOK)
        {
            IMU_CalibrateStart();
        }
    }
}

/**
  * @brief  控制信息页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_CtrlInfo(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
