#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_MainMenu(2);

/*贴图位置设置*/
#define PosCtrlInfo (screen.width()*0.25-15)
#define PosOptions  (screen.width()*0.75-15)
#define PosCursorY 55

/*实例化Ctrl图片控件*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Ctrl(&screen, (uint8_t*)gImage_Bitmap_Ctrl, 0, 0, 30, 30);

/*实例化Option图片控件*/
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Option(&screen, (uint8_t*)gImage_Bitmap_Option, 0, 0, 30, 30);

/*实例化光标控件*/
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCtrlInfo, PosCursorY, 30, 2);

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
static const float CursorTarget[] = {PosCtrlInfo, PosOptions};

/**
  * @brief  光标位置更新任务
  * @param  无
  * @retval 无
  */
static void Task_CursorUpdate()
{
    /*更新位置*/
    CursorPosNow = CursorTarget[CursorSelect % __Sizeof(CursorTarget)];
    
    if((CursorPosNow - CursorPosLast) != 0)
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2;

    MenuCursor.setPosition(CursorPosLast, PosCursorY);//设置控件位置
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{    
    /*播放载入动画*/
    for(float i = 0.7; i < 1.0; i += 0.05)
    {
        Bitmap_Ctrl.setPosition_f(0.5 - 0.25 * i, 0.6);
        Bitmap_Option.setPosition_f(0.5 + 0.25 * i, 0.6);
        if((ecd != 0 || btEcd || btLEFT || btRIGHT))
        {
            i = 1.0;
            Bitmap_Ctrl.setPosition_f(0.25, 0.6);
            Bitmap_Option.setPosition_f(0.75, 0.6);
            break;
        }
    }

    /*光标弹出动画*/
    for(float y = screen.height(); y > PosCursorY; y --)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        delay(10);
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
    for(float y = PosCursorY; y < screen.height() + 1; y ++)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        PageDelay(10);
    }
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    if(btLEFT && CursorSelect > 0)CursorSelect--;
    if(btRIGHT && CursorSelect < 1)CursorSelect++;

    if((btEcd || btUP || btDOWN))
    {
        if(CursorPosNow == PosCtrlInfo)
        {
            CursorPosLast = PosCtrlInfo;
            if(State_Handshake)
                page.PageChangeTo(PAGE_Handshake);
            else
                page.PageChangeTo(PAGE_CtrlInfo);
        }
        else if(CursorPosNow == PosOptions)
        {
            CursorPosLast = PosOptions;
            page.PageChangeTo(PAGE_Options);
        }
    }

}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
}

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
static void EncoderEvent()
{
    if(ecd < 0 && CursorSelect > 0)
        CursorSelect--;
    
    if(ecd > 0 && CursorSelect < 1)
        CursorSelect++;
}

/**
  * @brief  主菜单页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
