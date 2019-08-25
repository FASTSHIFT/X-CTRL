#include "FileGroup.h"
#include "fonts/FreeSerifItalic12pt7b.h"
MillisTaskManager mtmMainMenu(10);
static uint8_t MenuFlag=1;


static void Setup()
{
	tft.clear();
	tft.setCursor(45,30);
	tft.setFont(&FreeSerifItalic12pt7b);
	tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
	tft.setTextSize(1);
	tft.print("CtrlInfo");
	tft.setCursor(50,60);
	tft.print("Option");
	//tft.drawRGBBitmap(60,60,(uint8_t*)Ctrlinfo_bitmap,56,60);
}

static void Loop()
{
	static uint8_t n=0;
	if(millis()%50==0)
	{
		static uint8_t i=0,j=0,m=0;
		static bool f=1;
		if(f)
			i++;
		else
			i--;
	
		tft.drawTriangle(32-i,22+n,32-i,36+n,39-i,29+n,ST7735_WHITE);
		tft.drawTriangle(32-j,22+m,32-j,36+m,39-j,29+m,ST7735_BLACK);
		if(i>=10)
		{
			f=0;
		}
		if(i<=1)
			f=1;
		j=i;
		m=n;
	}
	
	if(MenuFlag==1)
		n=0;
	else if(MenuFlag==2)
		n=24;
	
	if(MenuFlag>2)
		MenuFlag=1;
	else if(MenuFlag<1)
		MenuFlag=2;
}

static void Exit()
{
	tft.clear();
}

static void ButtonPressEvent()
{
	if(btB)
	{
		if(MenuFlag==1)
			pm.PageChangeTo(PAGE_CtrlInfo);
		else if(MenuFlag==2)
			pm.PageChangeTo(PAGE_Option);
	}
	if(btUP)
	{
		MenuFlag++;
	}
	if(btDOWN)
	{
		MenuFlag--;
	}
}

static void ButtonLongPressEvent()
{
}

void PageRegister_MainMenu()
{
	#define ThisPage PAGE_MainMenu
	pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
	pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
