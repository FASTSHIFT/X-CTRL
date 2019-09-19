#include "FileGroup.h"
#include "GUI_Private.h"

#define BV_HEIGHT 36
#define BV_WIDTH  64

bool State_BV_Enable = false;

static uint8_t BV_Buffer[BV_HEIGHT * BV_WIDTH * 2];

BV_Player player1(BV_Buffer, sizeof(BV_Buffer), 0, StatusBar_POS + (screen.height() - StatusBar_POS - BV_HEIGHT) / 2);
BV_Player player2(BV_Buffer, sizeof(BV_Buffer), screen.width() - BV_WIDTH - 1, player1.BaseY);

static void Screen_FastDrawRGBBitmap(uint16_t *bitmap, BvHeader_TypeDef* head, int16_t *x, int16_t *y)
{
    uint32_t size = head->Width * head->Height;
    screen.setAddrWindow(*x, *y, head->Width - 1 + *x, head->Height - 1 + *y);
    for(uint32_t i = 0; i < size; i++)
        screen.pushColor(bitmap[i]);
}

static void Screen_FastDrawBitmap(uint8_t *bitmap, BvHeader_TypeDef* head, int16_t *x, int16_t *y)
{
    screen.setAddrWindow(*x, *y, head->Width - 1 + *x, head->Height - 1 + *y);

    int16_t byteWidth = (head->Width + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t b = 0;

    for(int16_t j = 0; j < head->Height; j++)
    {
        for(int16_t i = 0; i < head->Width; i++ )
        {
            if(i & 7) b <<= 1;
            else      b   = bitmap[j * byteWidth + i / 8];
            screen.pushColor((b & 0x80) ? screen.White : screen.Black);
        }
    }
}

bool Init_BvPlayer()
{
    Debug_SERIAL.println("Init BvPlayer...");

    player1.CallBackDrawPicture_u16 = Screen_FastDrawRGBBitmap;
    player2.CallBackDrawPicture_u16 = Screen_FastDrawRGBBitmap;

    player1.CallBackDrawPicture_u8 = Screen_FastDrawBitmap;
    player2.CallBackDrawPicture_u8 = Screen_FastDrawBitmap;

    State_BV_Enable = player1.OpenVideo("ICON_1.bv");
    State_BV_Enable = player2.OpenVideo("ICON_2.bv");

    player1.SetPlayerState(player1.Always);
    player2.SetPlayerState(player2.Always);
	
		State_BV_Enable = false;

    return State_BV_Enable;
}
