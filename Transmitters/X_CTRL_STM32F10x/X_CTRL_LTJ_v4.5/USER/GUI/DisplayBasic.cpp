#include "FileGroup.h"
#include "GUI_Private.h"

PageManager page(PAGE_MAX, EVENT_MAX);
SCREEN_CLASS screen(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
#define TFT_BLK    PA15
void Init_Display()
{
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);
    screen.initR(INITR_MINI160x80);
    screen.setRotation(3);
    screen.fillScreen(screen.Black);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextWrap(true);
    screen.setTextSize(1);
}

void Init_Pages()
{
    PageRegister_MainMenu(PAGE_MainMenu);
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
    PageRegister_SetGravity(PAGE_SetGravity);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
}

void Init_GUI()
{
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);
    Init_HMI();
    screen.clear();
    Init_StatusBar();
    Init_Pages();
}

void PageDelay(uint32_t ms)
{
    uint32_t Stop_TimePoint = millis() + ms;
    while(millis() < Stop_TimePoint)
        Thread_StatusBar();
}

void Thread_GUI()
{
    Thread_StatusBar();
    page.Running();
}
