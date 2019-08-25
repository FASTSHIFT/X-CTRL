#include "FileGroup.h"
#include "GUI_Private.h"

void FastDrawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    uint32_t size = w * h;
    screen.setAddrWindow(x, y, x + w - 1, y + h - 1);
    for(uint32_t i = 0; i < size; i++)
        screen.pushColor(bitmap[i]);
}

void Draw_RGBBitmap_BasicPage(float pos_x1, float pos_x2, float pos_y)
{
    screen.drawRGBBitmap(screen.width() * pos_x1 - 15, screen.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Ctrl, 30, 30);
    screen.drawRGBBitmap(screen.width() * pos_x2 - 15, screen.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Option, 30, 30);
}

void Draw_RGBBitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y)
{
    screen.drawRGBBitmap(screen.width() * pos_x1 - 15, screen.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_RC, 30, 30);
    screen.drawRGBBitmap(screen.width() * pos_x2 - 15, screen.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Common, 30, 30);
    screen.drawRGBBitmap(screen.width() * pos_x3 - 15, screen.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_DS, 30, 30);

    screen.drawLine(screen.width() * pos_x1 - 15, screen.height() * pos_y - 18, screen.width() * pos_x3 + 15, screen.height() * pos_y - 18, screen.White);
}

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir)
{
    const unsigned char* gImage_Bitmap_Logox[] = {
        gImage_Bitmap_Logo1,
        gImage_Bitmap_Logo2,
        gImage_Bitmap_Logo3,
        gImage_Bitmap_Logo4,
        gImage_Bitmap_Logo5,
        gImage_Bitmap_Logo6,
        gImage_Bitmap_Logo7,
        gImage_Bitmap_Logo8,
        gImage_Bitmap_Logo9,
        gImage_Bitmap_Logo10,
        gImage_Bitmap_Logo11,
        gImage_Bitmap_Logo12,
    };

    if(dir)
    {
        for(uint8_t i = 12; i > 0; i--)
        {
            screen.drawRGBBitmap(screen.width() * x - 93 / 2, screen.height() * y - 5, (uint16_t*)gImage_Bitmap_Logox[i], 93, 10);
            delay(time);
        }
    }
    else
    {
        for(uint8_t i = 0; i < 12; i++)
        {
            screen.drawRGBBitmap(screen.width() * x - 93 / 2, screen.height() * y - 5, (uint16_t*)gImage_Bitmap_Logox[i], 93, 10);
            delay(time);
        }
    }
}
