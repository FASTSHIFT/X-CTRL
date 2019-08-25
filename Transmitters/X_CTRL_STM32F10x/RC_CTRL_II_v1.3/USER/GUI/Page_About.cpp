#include "FileGroup.h"
#include "GUI_Private.h"

static void Setup()
{
    oled.setTextColor(WHITE);
    oled.setTextSize(2);
    oled.setCursor(10, 18);
    oled.print("VIFEXTech");
    oled.setTextSize(1);
    oled.setCursor(33, 40);
    oled << "RC-CTRL " << _RC_CTRL_VERSION;
    oled.setCursor(13, 50);
    oled << __DATE__ << " Build";
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
