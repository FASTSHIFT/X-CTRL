#include "FileGroup.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

void Task_TFT_Display()
{
	static uint32_t FPS;
	static uint8_t L_Circle_X_Last, L_Circle_Y_Last;
    static uint8_t R_Circle_X_Last, R_Circle_Y_Last;

    tft.setCursor(0, 0);
    tft << FPS << "FPS ";

    uint8_t L_Circle_X = map(JS_L.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 6, 43);
    uint8_t L_Circle_Y = map(-JS_L.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 14, 51);
    uint8_t R_Circle_X = map(JS_R.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 127 - 50 + 6, 127 - 7);
    uint8_t R_Circle_Y = map(-JS_R.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 14, 51);

    if(L_Circle_X != L_Circle_X_Last || L_Circle_Y != L_Circle_Y_Last)
    {
        tft.fillCircle(L_Circle_X_Last, L_Circle_Y_Last, 5, ST7735_BLACK);
        tft.fillCircle(L_Circle_X, L_Circle_Y, 5, ST7735_WHITE);
        L_Circle_X_Last = L_Circle_X;
        L_Circle_Y_Last = L_Circle_Y;
    }
    if(R_Circle_X != R_Circle_X_Last || R_Circle_Y != R_Circle_Y_Last)
    {
        tft.fillCircle(R_Circle_X_Last, R_Circle_Y_Last, 5, ST7735_BLACK);
        tft.fillCircle(R_Circle_X, R_Circle_Y, 5, ST7735_WHITE);
        R_Circle_X_Last = R_Circle_X;
        R_Circle_Y_Last = R_Circle_Y;
    }
	
	static uint32_t TimePoint, LoopCnt;
    LoopCnt++;
    if(millis() > TimePoint)
    {
        FPS = LoopCnt;
        LoopCnt = 0;
        TimePoint = millis() + 1000;
    }
}
