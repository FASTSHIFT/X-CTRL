#include "FileGroup.h"
#include "GUI_Private.h"
#include "FreeSerif9pt7b.h"

static void Setup()
{
    screen.setTextColor(screen.White);
    screen.setTextSize(0);
    screen.setFont(&FreeSerif9pt7b);
    screen.setCursor(20, 32);//10,18
    screen.print("VIFEXTech");

    screen.setFont();
    screen.setTextSize(1);
    screen.setCursor(33, 40);
    screen << "X-CTRL " << _X_CTRL_VERSION;
    screen.setCursor(13, 50);
    screen << __DATE__ << " Build";

    XFS_Speak("X-Control固件版本:%s,编译时间:%s,%s", _X_CTRL_VERSION, __DATE__, __TIME__);
    XFS_Speak("设计者:VIFAX Technology");
}

static void Loop()
{
}

static void Exit()
{
}

static void ButtonPressEvent()
{
    page.PageChangeTo(PAGE_Options);
}

void PageRegister_About(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
