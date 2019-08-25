#include "FileGroup.h"
#include "GUI_Private.h"

static uint8_t BV_Buffer[64 * 36 * 2];

BV_Player player1(BV_Buffer, sizeof(BV_Buffer), 0, 55);
BV_Player player2(BV_Buffer, sizeof(BV_Buffer), 65, 55);

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

void Init_BvPlayer()
{
    Serial.println("Init BvPlayer...");

    player1.CallBackDrawPicture_u16 = Screen_FastDrawRGBBitmap;
    player2.CallBackDrawPicture_u16 = Screen_FastDrawRGBBitmap;

    player1.CallBackDrawPicture_u8 = Screen_FastDrawBitmap;

    player1.OpenVideo("ICON_1.bv");
    player2.OpenVideo("ICON_2.bv");

    player1.SetPlayerState(player1.Always);
    player2.SetPlayerState(player2.Always);

//    while(1)
//    {
//        player1.UpdatePlayerState();
//        player2.UpdatePlayerState();
//        player3.UpdatePlayerState();
//        player4.UpdatePlayerState();
//    }
}
