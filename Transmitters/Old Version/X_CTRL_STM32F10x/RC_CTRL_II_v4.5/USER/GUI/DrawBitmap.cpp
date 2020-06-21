#include "FileGroup.h"
#include "GUI_Private.h"

void Draw_Bitmap_BasicPage(float pos_x1, float pos_x2, float pos_y)
{
    screen.drawBitmap(screen.width() * pos_x1 - 15, screen.height() * pos_y - 15, gImage_Bitmap_Ctrl, 30, 30, screen.White, screen.Black);
    screen.drawBitmap(screen.width() * pos_x2 - 15, screen.height() * pos_y - 15, gImage_Bitmap_Option, 30, 30, screen.White, screen.Black);
}

void Draw_RGBBitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y)
{
    screen.drawBitmap(screen.width() * pos_x1 - 15, screen.height() * pos_y - 15, gImage_Bitmap_RC, 30, 30, screen.White, screen.Black);
    screen.drawBitmap(screen.width() * pos_x2 - 15, screen.height() * pos_y - 15, gImage_Bitmap_Common, 30, 30, screen.White, screen.Black);
    screen.drawBitmap(screen.width() * pos_x3 - 15, screen.height() * pos_y - 15, gImage_Bitmap_DS, 30, 30, screen.White, screen.Black);

    screen.drawLine(screen.width() * pos_x1 - 15, screen.height() * pos_y - 18, screen.width() * pos_x3 + 15, screen.height() * pos_y - 18, screen.White);
}

