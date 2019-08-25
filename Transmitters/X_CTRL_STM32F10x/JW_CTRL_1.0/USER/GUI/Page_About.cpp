#include "FileGroup.h"
#include "GUI_Private.h"
#include "fonts/FreeSerif9pt7b.h"

static void Setup()
{
    screen.setTextColor(screen.White);
    screen.setTextSize(0);
    screen.setFont(&FreeSerif9pt7b);
    screen.setCursor(20, 32);//10,18
    screen.print("VIFEXTech");

    screen.setFont();
    screen.setTextSize(1);
    
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
    page.PageChangeTo(PAGE_Options);
}

void PageRegister_About()
{
#define ThisPage PAGE_About
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
//	page.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
//	page.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
