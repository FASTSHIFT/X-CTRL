#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_MainMenu(2);

/*贴图位置设置*/
#define PosCtrlInfo 0.25f
#define PosOptions  0.75f
#define PosCursorY  0.8f

/*实例化Ctrl图片控件*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Ctrl(&screen, (uint16_t*)gImage_Bitmap_Ctrl, 0, 0, 30, 30);

/*实例化Option图片控件*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Option(&screen, (uint16_t*)gImage_Bitmap_Option, 0, 0, 30, 30);

/*实例化光标控件*/
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCursorY * screen.height(), 100, Bitmap_Ctrl.Width, 2);

#define CursorCloseTo(pos) (ABS(CursorPosNow - pos)<0.01f)

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
static const float CursorTargetNow[] = {PosCtrlInfo, PosOptions};
static uint32_t StayPageTimePoint;
static bool IsSpoke;

/**
  * @brief  光标位置更新任务
  * @param  无
  * @retval 无
  */
static void Task_CursorUpdate()
{
    /*更新位置*/
    CursorPosNow = CursorTargetNow[CursorSelect % __Sizeof(CursorTargetNow)];

    if(!CursorCloseTo(CursorPosLast))
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2f;

    MenuCursor.setPosition_f(CursorPosLast, PosCursorY); //设置控件位置
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    StayPageTimePoint = millis();//记录页面进入时间戳
    IsSpoke = false;

    /*播放载入动画*/
    for(float i = 0.7f; i < 1.0f; i += 0.05f)
    {
        Bitmap_Ctrl.setPosition_f(0.5f - 0.25f * i, 0.6f);
        Bitmap_Option.setPosition_f(0.5f + 0.25f * i, 0.6f);
        if((ecd != 0 || btEcd || btOK || btBACK))
        {
            i = 1.0;
            Bitmap_Ctrl.setPosition_f(PosCtrlInfo, 0.6f);
            Bitmap_Option.setPosition_f(PosOptions, 0.6f);
            break;
        }
    }

    /*光标弹出动画*/
    for(float y = 1.1f; y > PosCursorY; y -= 0.01f)
    {
        MenuCursor.setPosition_f(CursorPosLast, y);
        PageDelay(10);
    }

    /*任务注册*/
    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 20);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    if(!IsSpoke && millis() - StayPageTimePoint > 10000)
    {
        if(CTRL.Battery.Usage < 30)
            XFS_Speak("警告：电池电量低。");

        XFS_Speak(
            "当前电池电压%0.1f伏，剩余电量%0.0f%%",
            CTRL.Battery.Voltage,
            CTRL.Battery.Usage
        );

        if(CTRL.Battery.Usage < 20)
            XFS_Speak("请立即充电");
        else if(CTRL.Battery.Usage < 30)
            XFS_Speak("请及时充电");

        IsSpoke = true;
    }
    mtm_MainMenu.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    /*光标下落动画*/
    for(float y = PosCursorY; y < 1.1f; y += 0.01f)
    {
        MenuCursor.setPosition_f(CursorPosLast, y);
        PageDelay(10);
    }
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(param == &ecd)
    {
        if(ecd < 0 && CursorSelect > 0)
            CursorSelect--;

        if(ecd > 0 && CursorSelect < 1)
            CursorSelect++;
    }

    if(event == EVENT_ButtonPress)
    {
        if(btDOWN && CursorSelect > 0)CursorSelect--;
        if(btUP && CursorSelect < 1)CursorSelect++;

        if(btEcd || btOK)
        {
            if(CursorCloseTo(PosCtrlInfo))
            {
                CursorPosLast = PosCtrlInfo;
                if(CTRL.State.Handshake)
                    page.PagePush(PAGE_Handshake);
                else
                    page.PagePush(PAGE_CtrlInfo);
            }
            else if(CursorCloseTo(PosOptions))
            {
                CursorPosLast = PosOptions;
                page.PagePush(PAGE_Options);
            }
        }
    }
}

/**
  * @brief  主菜单页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_MainMenu(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
