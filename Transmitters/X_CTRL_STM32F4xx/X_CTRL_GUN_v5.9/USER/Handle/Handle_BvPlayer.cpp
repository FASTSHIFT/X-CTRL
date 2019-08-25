#include "FileGroup.h"
#include "GUI_Private.h"

static uint8_t BV_Buffer1[64 * 36 * 2];
static uint8_t BV_Buffer2[64 * 36 * 2];

BV_Player playerCtrl_1(BV_Buffer1, sizeof(BV_Buffer1), 0, 55);
BV_Player playerCtrl_2(BV_Buffer1, sizeof(BV_Buffer1), 0, 55);

BV_Player playerOption_1(BV_Buffer2, sizeof(BV_Buffer2), 65, 55);
BV_Player playerOption_2(BV_Buffer2, sizeof(BV_Buffer2), 65, 55);

static void Screen_FastDrawRGBBitmap(uint16_t *bitmap, BvHeader_TypeDef* head, int16_t *x, int16_t *y)
{
    uint32_t size = head->Width * head->Height;
    screen.setAddrWindow(*x, *y, head->Width - 1 + *x, head->Height - 1 + *y);
    for(uint32_t i = 0; i < size; i++)
        screen.pushColor(bitmap[i]);
//	screen.drawRGBBitmapFast(*x, *y, bitmap, head->Width, head->Height);
}

//static void Screen_FastDrawBitmap(uint8_t *bitmap, BvHeader_TypeDef* head, int16_t *x, int16_t *y)
//{
//    screen.setAddrWindow(*x, *y, head->Width - 1 + *x, head->Height - 1 + *y);

//    int16_t byteWidth = (head->Width + 7) / 8; // Bitmap scanline pad = whole byte
//    uint8_t b = 0;

//    for(int16_t j = 0; j < head->Height; j++)
//    {
//        for(int16_t i = 0; i < head->Width; i++ )
//        {
//            if(i & 7) b <<= 1;
//            else      b   = bitmap[j * byteWidth + i / 8];
//            screen.pushColor((b & 0x80) ? screen.White : screen.Black);
//        }
//    }
//}

void Init_BvPlayer()
{
    Serial.println("Init BvPlayer...");

    playerCtrl_2.CallBackDrawPicture_u16 = playerCtrl_1.CallBackDrawPicture_u16 = Screen_FastDrawRGBBitmap;
    playerOption_2.CallBackDrawPicture_u16 = playerOption_1.CallBackDrawPicture_u16 = Screen_FastDrawRGBBitmap;

    playerCtrl_1.OpenVideo("CTRL_2.bv");
	playerCtrl_2.OpenVideo("CTRL_1.bv");
	playerOption_1.OpenVideo("CTRL_2.bv");
	playerOption_2.OpenVideo("CTRL_1.bv");

    playerCtrl_1.SetPlayerState(playerCtrl_1.Run);
	playerCtrl_2.SetPlayerState(playerCtrl_2.Stop);
	playerOption_1.SetPlayerState(playerOption_1.Run);
	playerOption_2.SetPlayerState(playerOption_2.Stop);
}
