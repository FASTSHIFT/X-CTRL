#include "FileGroup.h"

static void Setup()
{
	tft.clear();
	tft.setCursor(45,30);
	tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
	tft.setTextSize(1);
	tft.print("......");
}

static void Loop()
{
	
}

static void Exit()
{
	
}

static void ButtonPressEvent()
{
	if(btB)
	{
		pm.PageChangeTo(PAGE_MainMenu);
	}
}

static void ButtonLongPressEvent()
{
}

void PageRegister_Option()
{
	#define ThisPage PAGE_Option
	pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
	pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
