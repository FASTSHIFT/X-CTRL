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
}

static void Loop()
{
}

static void Exit()
{
}

static void ButtonPressEvent()
{
    pm.PageChangeTo(PAGE_Options);
}

void PageRegister_About()
{
#define ThisPage PAGE_About
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
//	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
//	pm.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
