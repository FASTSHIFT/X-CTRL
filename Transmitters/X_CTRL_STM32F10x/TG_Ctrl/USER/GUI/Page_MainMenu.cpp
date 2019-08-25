#include "FileGroup.h"
#include "GUI_Private.h"

HMI::Widget text(&HMI_SERIAL, "tSPD.val=");


/********** 基本 ************/
static void Setup()
{
    //页面初始化事件
    text.SetValue("%d",0);
}

static void Loop()
{
    //页面循环事件
}

static void Exit()
{
    //页面退出事件
}

/************ 事件处理 **********/
static void ButtonPressEvent()
{
    //按键按下事件
}

static void ButtonLongPressEvent()
{
    //按键长摁事件
}

static void EncoderEvent()
{
    //旋转编码器事件
}

/************** 页面注册 **************/
void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}

