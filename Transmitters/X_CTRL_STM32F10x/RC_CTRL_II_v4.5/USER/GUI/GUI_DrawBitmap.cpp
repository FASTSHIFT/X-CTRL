#include "FileGroup.h"
#include "GUI_Private.h"

void Draw_RGBBitmap_BasicPage(float pos_x1, float pos_x2, float pos_y)
{
    oled.drawBitmap(oled.width() * pos_x1 - 15, oled.height() * pos_y - 15, gImage_Bitmap_Ctrl, 30, 30, WHITE, BLACK);
    oled.drawBitmap(oled.width() * pos_x2 - 15, oled.height() * pos_y - 15, gImage_Bitmap_Option, 30, 30, WHITE, BLACK);
}

void Draw_RGBBitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y)
{
    oled.drawRGBBitmap(oled.width() * pos_x1 - 15, oled.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_RC, 30, 30);
    oled.drawRGBBitmap(oled.width() * pos_x2 - 15, oled.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_Common, 30, 30);
    oled.drawRGBBitmap(oled.width() * pos_x3 - 15, oled.height() * pos_y - 15, (uint16_t*)gImage_Bitmap_DS, 30, 30);

    oled.drawLine(oled.width() * pos_x1 - 15, oled.height() * pos_y - 18, oled.width() * pos_x3 + 15, oled.height() * pos_y - 18, WHITE);
}

void Draw_RGBBitmap_Logo(float x, float y, uint16_t time, bool dir)
{
}
