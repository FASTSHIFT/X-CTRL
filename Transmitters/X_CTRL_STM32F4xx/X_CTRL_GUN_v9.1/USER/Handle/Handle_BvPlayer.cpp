#include "FileGroup.h"
#include "GUI_Private.h"
#include "FileSystem.h"

bool State_BV_Enable = false;

#define BV_HEIGHT 36
#define BV_WIDTH 64

uint8_t BV_Buffer[8 * 1024];

BV_Player player1(BV_Buffer, sizeof(BV_Buffer), 0, StatusBar_POS + (screen.height() - StatusBar_POS - BV_HEIGHT) / 2);
BV_Player player2(BV_Buffer, sizeof(BV_Buffer), screen.width() - BV_WIDTH - 1, player1.BaseY);
BV_Player BvPlayer(BV_Buffer, sizeof(BV_Buffer), 0, StatusBar_POS + 1);

static void Screen_FastDrawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    screen.drawFastRGBBitmap(x, y, bitmap, w, h);
//    static uint16_t color;
//    screen.drawRect(x, y, w, h, color+=10);
}

static void Screen_FastDrawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h)
{
    screen.drawFastBitmap(x, y, bitmap, w, h);
}


bool Init_BvPlayer()
{
    DEBUG_FUNC();

    player1.DrawPic_16bit = Screen_FastDrawRGBBitmap;
    player2.DrawPic_16bit = Screen_FastDrawRGBBitmap;
    BvPlayer.DrawPic_16bit = Screen_FastDrawRGBBitmap;

    player1.DrawPic_8bit = Screen_FastDrawBitmap;
    player2.DrawPic_8bit = Screen_FastDrawBitmap;
    BvPlayer.DrawPic_8bit = Screen_FastDrawBitmap;

    State_BV_Enable = (player1.OpenVideo("ICON_1.bv") && player2.OpenVideo("ICON_2.bv"));

    player1.SetPlayerState(player1.Always);
    player2.SetPlayerState(player2.Always);
    BvPlayer.SetPlayerState(BvPlayer.Always);

    return State_BV_Enable;
}
