#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "LightGUI.h"
#include "PageManager.h"
#include "Bitmaps.h"
#include "TFT_SSD1283A.h"
#include "BV_Player.h"

#define SCREEN_CLASS TFT_SSD1283A

#define StatusBar_POS 17
#define ClearDisplay()      screen.fillScreen(screen.Black)
#define ClearPage()         screen.fillRect(0,StatusBar_POS+1,screen.width(),screen.height()-(StatusBar_POS+1),screen.Black)
#define ClearStatusBar()    screen.fillRect(0,0,screen.width(),StatusBar_POS-1,screen.Black)

extern SCREEN_CLASS screen;
extern PageManager page;

typedef enum {
    PAGE_MainMenu,
    PAGE_MAX
} PageNum_Type;

typedef enum {
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_ButtonRelease,
    EVENT_EncoderRotate,
    EVENT_MAX
} Event_Type;

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir = 0);

void Init_StatusBar();
void Thread_StatusBar();
void PageDelay(uint32_t ms);

#endif
