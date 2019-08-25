#include "FileGroup.h"
#include "Bitmap.h"
#include "GUI_Private.h"

void Draw_RGBBitmap_BasicPage(float pos_x1, float pos_x2, float pos_y)
{
    tft.drawRGBBitmap(tft.width() * pos_x1 - 15, tft.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Ctrl, 30, 30);
    tft.drawRGBBitmap(tft.width() * pos_x2 - 15, tft.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Option, 30, 30);
}

void Draw_RGBBitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y)
{
    tft.drawRGBBitmap(tft.width() * pos_x1 - 15, tft.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_RC, 30, 30);
    tft.drawRGBBitmap(tft.width() * pos_x2 - 15, tft.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Common, 30, 30);
    tft.drawRGBBitmap(tft.width() * pos_x3 - 15, tft.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_DS, 30, 30);

    tft.drawLine(tft.width() * pos_x1 - 15, tft.height() * pos_y - 18, tft.width() * pos_x3 + 15, tft.height() * pos_y - 18, ST7735_WHITE);
    AnimationBezierLine.ControlP1_X = 0.5;
    AnimationBezierLine.ControlP1_Y = -1;
    AnimationBezierLine.ControlP2_X = 0.5;
    AnimationBezierLine.ControlP2_Y = 2;
}

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time)
{
    for(uint8_t i = 0; i < 12; i++)
    {
        tft.drawRGBBitmap(tft.width() * x - 93 / 2, tft.height() * y - 5, (uint16_t*)gImage_Bitmap_Logox[i], 93, 10);
        delay(time);
    }
}
