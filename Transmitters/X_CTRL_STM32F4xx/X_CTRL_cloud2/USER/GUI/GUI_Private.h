#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "PageManager.h"
#include "TFT_S6B33B2.h"
#include "LightGUI.h"

#define SCREEN_CLASS TFT_S6B33B2

#define TaskBarHeight					18
#define ContentArea_Y					20
#define TextHeight						15
#define TextHeight_Light			11
#define TextHeight_Bold				20

#define ClearDisplay()      screen.fillScreen(screen.Black)

extern SCREEN_CLASS screen;
extern PageManager page;



typedef enum 
{
	PAGE_CTRL,
	PAGE_MAIN,
	PAGE_MAX
} PageNum_Type;

typedef enum 
{
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_ButtonRelease,
    EVENT_EncoderRotate,
    EVENT_MAX
} Event_Type;

void PageRegister_Ctrl(uint8_t id);
void PageRegister_Main(uint8_t id);


void Page_ClearScreen(uint16_t color);
void Page_DrawTaskBar();
void Page_DrawBattery();

#endif
