
/*页面模板*/

#include "filegroup.h"
#include "buttons.h"
#include "GUI_Private.h"

#define RGB888To565(r, g, b)	( ((uint16_t)(r >> 3) << 11) | ((uint16_t)(g >> 2) << 5) | ((uint16_t)(b >> 2)) )

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
//	while(true)
//	{
//		screen.setCursor(0, 0);
//		screen.setTextColor(screen.White, screen.Black);
//		screen.SetFontX(Fonts_MicrosoftYahei11px);
//		screen.printfX("%d  ", Buttons_IsLongPressed(1));
//		delay(100);
//	}
	
	screen.fillScreen(screen.White);
	Page_DrawTaskBar();
	screen.setCursor(0, 0);
	screen.setTextColor(screen.White, screen.Black);
	screen.SetFontX(Fonts_MicrosoftYahei11px);
	screen.setCursor(0, 1);
	screen.printfX(" Connect to Device ...", millis());
}

static int FoundIndex = 0;
static bool Found = false;
static bool Connected = true;

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
	if(Connected)
	{
		screen.setTextColor(screen.Black, screen.White);
		
		screen.setCursor(0, ContentArea_Y + TextHeight * 0);
		screen.printfX("Client Initialized");
		
		screen.setCursor(0, ContentArea_Y + TextHeight * 1);
		screen.printfX("Client Connecting... ");
		
		do
		{
			client.ClearTargetNameStatus();
			while(!client.BeginGetTargetName());
			screen.setCursor(0, ContentArea_Y + TextHeight * 2);
			screen.printfX("Client Loading...  ");
			volatile uint32_t now = millis();
			volatile bool suc = false;
			while(millis() - now < 100)
			{
				Client_Handle();
				if(client.GetTargetNameStatus())
				{
					suc = true;
					break;
				}
			}
			if(suc)
			{
				break;
			}
		}
		while(1);
		
		screen.setCursor(0, ContentArea_Y + TextHeight * 3);
		char strName[32];
		client.GetTargetName(strName);
		screen.printfX("Name : %s", strName);
		
    LightGUI::ProgressBar<SCREEN_CLASS> loadingBar(&screen, 3, ContentArea_Y + TextHeight * 4 + 5, screen.width() - 20, 6, 0);
		loadingBar.Color_PB = screen.Black;
		loadingBar.Color_BG = screen.White;
		loadingBar.Color_FM = screen.Black;
		loadingBar.setProgress(0);
		
		screen.setCursor(0, ContentArea_Y + TextHeight * 5);
		screen.printfX("Load API List   ");
		uint8_t fCnt = 0;
		while(!client.BeginGetFuncCount());
		while(!client.GetFuncCountStatus())
		{
			Client_Handle();
		}
		fCnt = client.GetTargetFuncCount();
		
		for(int i = 0; i < fCnt; i++)
		{
			screen.setCursor(0, ContentArea_Y + TextHeight * 6);
			screen.printfX("Loading APIs (%d / %d)    ", i + 1, fCnt);
			
			char funName[32];
			do
			{
				client.ClearFuncNameStatus();
				while(!client.BeginGetFuncName(i));
				volatile uint32_t reqMS = millis();
				while(!client.GetFuncNameStatus() && millis() - reqMS < 100)
				{
					Client_Handle();
				}
				if(client.GetFuncNameStatus())
				{
					for(int i = 0; i < 32; i++)
					{
						funName[i] = 0;
					}
					client.GetTargetFuncName(i, funName);
					if(strlen(funName) > 2)
					{
						break;
					}
				}
			}
			while(true);
			
			screen.setCursor(0, ContentArea_Y + TextHeight * 7);
			screen.fillRect(0, ContentArea_Y + TextHeight * 7, screen.width(), TextHeight, screen.White);
			screen.printfX("-> %s", funName);
			
			//delay(100);
			
			loadingBar.setProgress((float)i / fCnt);
		}
		loadingBar.setProgress(1);
		screen.setCursor(0, ContentArea_Y + TextHeight * 8);
		screen.printfX("Load Completed");
		
		delay(100);
		
		page.PageChangeTo(PAGE_MAIN);
	}
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	
}

/************ 事件处理 **********/
/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
}

/**
  * @brief  按键长按事件
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
	if(ecd > 0)
	{
		
	}
	else
	{
		
	}
	
}

/**
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Ctrl(uint8_t ThisPage)
{
    /*基本*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    
    /*事件*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
