#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

#include "stdint.h"
#include "LightGUI.h"
#include "PageManager.h"
#include "MenuManager.h"
#include "Bitmaps.h"
#include "TFT_SSD1283A.h"
#include "BV_Player.h"
#include "ButtonEvent.h"

/*按键事件命名空间*/
using namespace ButtonEvent_Type;
/*屏幕类名*/
#define SCREEN_CLASS TFT_SSD1283A
extern SCREEN_CLASS screen;

/*Page管理器*/
extern PageManager page;

/*标准Adafruit等宽字体*/
#define TEXT_HEIGHT_1   8
#define TEXT_WIDTH_1    6

/*XWZ抗锯齿非等宽字体*/
#define TEXT_HEIGHT_2  14

/*状态栏高度*/
#define StatusBar_Height       (TEXT_HEIGHT_2+3)
#define StatusBar_Width        (screen.width())

/*Page*/
#define Page_Fill(color)     screen.fillRect(0,StatusBar_Height+1,screen.width(),screen.height()-(StatusBar_Height+1),color)
#define Page_Height          (screen.height() - StatusBar_Height)
#define Page_Width           (screen.width())

/*清全屏*/
#define ClearDisplay()      screen.fillScreen(screen.Black)
/*清Page*/
#define ClearPage()         Page_Fill(screen.Black)
/*清状态栏*/
#define ClearStatusBar()    screen.fillRect(0,0,screen.width(),StatusBar_Height+1,screen.Black)

/*设置默认字体状态*/
#define TextSetDefault() screen.setTextColor(screen.White,screen.Black),screen.setTextSize(1)
/*字符串中心坐标*/
#define TextMid(x,textnum) (screen.width()*(x)-(TEXT_WIDTH_1*(textnum))/2.0f)
/*字符串居中打印*/
#define TextMidPrint(x,y,text) screen.setCursor(TextMid((x),strlen(text)),screen.height()*(y)-TEXT_HEIGHT_1/2.0f),screen.print(text)
/*屏幕中心坐标*/
#define ScreenMid_W (screen.width()/2)
#define ScreenMid_H (screen.height()/2)



typedef enum
{
    /*保留*/
    PAGE_None,
    /*用户页面*/
    PAGE_About,
    PAGE_BvPlayer,
    PAGE_ChannelCfg,
    PAGE_CtrlInfo,
    PAGE_FreqGraph,
    PAGE_FileExplorer,
    PAGE_GameHopper,
    PAGE_GameDoom,
    PAGE_Handshake,
    PAGE_Jamming,
    PAGE_LuaScript,
    PAGE_MainMenu,
    PAGE_Options,
    PAGE_SetJoystick,
    PAGE_SetBluetooth,
    PAGE_SetGravity,
    PAGE_WavPlayer,
    /*保留*/
    PAGE_MAX
} PageNum_Type;

#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)

void PageRegister_MainMenuDymanic(uint8_t pageID);
void PageRegister_MainMenu(uint8_t pageID);

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir = 0);
void Draw_RFScaleplate(int16_t x, int16_t y, int16_t length, uint16_t color);

void Init_StatusBar();
void Thread_StatusBar();
void PageDelay(uint32_t ms);
void StatusBarDelay(uint32_t ms);
void CloseStatusBar();

extern BV_Player playerCtrl_1, playerCtrl_2;
extern BV_Player playerOption_1, playerOption_2;

extern BV_Player player1, player2, BvPlayer;


#endif
