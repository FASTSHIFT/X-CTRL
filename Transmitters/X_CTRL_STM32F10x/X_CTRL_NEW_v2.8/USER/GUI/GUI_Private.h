
#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "Bitmap.h"
#include "GUI_Widget.h"
#include "PageManager.h"

#define StatusBar_POS 12
#define clear() fillScreen(screen.Black)
#define clearPage() fillRect(0,StatusBar_POS+1,screen.width(),screen.height()-(StatusBar_POS+1),screen.Black)

typedef enum{
    PAGE_MainMenu,
    PAGE_CtrlInfo,
    PAGE_Options,
    PAGE_SetJoystick,
    PAGE_SetBluetooth,
	PAGE_SetGravity,
    PAGE_About,
    PAGE_Handshake,
	PAGE_END
}PageNum_Type;

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir = 0);
void Draw_RGBBitmap_BasicPage(float pos_x1, float pos_x2, float pos_y);
void Draw_RGBBitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y);
void FastDrawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);

void Init_StatusBar();

#endif
