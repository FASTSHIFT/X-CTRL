#include "FileGroup.h"
#include "GUI_Private.h"

PageManager pm(PAGE_END);
Adafruit_ST7735 screen = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

void Init_Display()
{
    screen.initR(INITR_MINI128x64);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(true);
    screen.setTextSize(0);
}

void Init_GUI()
{
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);
	Init_HMI();
    screen.clear();
    Init_StatusBar();
    PageRegister_MainMenu();
    PageRegister_CtrlInfo();
    PageRegister_Options();
    PageRegister_SetJoystick();
    PageRegister_SetBluetooth();
    PageRegister_SetGravity();
    PageRegister_About();
    PageRegister_Handshake();
}
