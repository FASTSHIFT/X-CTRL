#include "FileGroup.h"
#include "GUI_Private.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
MillisTaskManager mtm_Display(10);

uint16_t TFT_FPS;

typedef struct
{
    uint8_t Pos;
    uint8_t Cnt;
    uint8_t Points[128];
    uint8_t Now;
    bool IsCompleted;
} CursorMove_t;

CursorMove_t CM;
BezierLine AnimationBezierLine;

void Task_CursorMoveUpdate()
{
    if(!CM.IsCompleted)
    {
        if(CM.Pos < CM.Cnt)
        {
            CM.Now = CM.Points[CM.Pos++];
        }
        else
        {
            CM.IsCompleted = true;
        }
    }
}

void Draw_Cursor(uint8_t x, uint8_t y, uint16_t color)
{
    x -= 15;
    static uint8_t last_x, last_y;
    if(x != last_x || y != last_y)
    {
        tft.fillRect(last_x, last_y, 30, 2, ST7735_BLACK);
        tft.fillRect(x, y, 30, 2, color);
        last_x = x, last_y = y;
    }
}

int GetMenuPosition(int menuSelect)
{
    switch(menuSelect % 3)
    {
    case 0:
        return 127 * 0.25;
    case 1:
        return 127 * 0.5;
    case 2:
        return 127 * 0.75;
    }
    return 0;
}

void Draw_JoystickPos()
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
}

void Task_ShowFPS()
{
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.printf("%-3d", TFT_FPS);
    TFT_FPS = 0;

    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextSize(2);
}

uint8_t MenuSelect = 1;

void Menu_Up()
{
    MenuSelect = (MenuSelect + 1) % 3;
}

void Menu_Down()
{
    MenuSelect = (MenuSelect + 2) % 3;
}

void Task_TFT_Display()
{
    static uint8_t LastMenuSelect = 0;
    tft.setCursor(20, 2);
    switch(MenuSelect % 3)
    {
    case 0:
        //tft.setTextColor(ST7735_RED, ST7735_BLACK);
        tft.print("R/C");
        break;
    case 1:
        //tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
        tft.print("COM");
        break;
    case 2:
        //tft.setTextColor(ST7735_BLUE, ST7735_BLACK);
        tft.print("TAK");
        break;
    }

    if(MenuSelect != LastMenuSelect)
    {
        int cnt = 0;
        int last = GetMenuPosition(LastMenuSelect);
        int now = GetMenuPosition(MenuSelect);
        int delta = now > last ? 1 : -1;
        AnimationBezierLine.From = AnimationBezierLine.MinStep = last;
        AnimationBezierLine.To = AnimationBezierLine.MaxStep = now;
        for(int i = last; i != now; i += delta)
        {
            CM.Points[cnt ++] = AnimationBezierLine.Get(i);
        }
        LastMenuSelect = MenuSelect;
        CM.Pos = 0;
        CM.Cnt = cnt;
        CM.IsCompleted = false;
    }
    Draw_Cursor(CM.Now, 55, ST7735_WHITE);
    TFT_FPS++;
}

void Task_GUI_Wigdet_Update()
{
    Draw_BattUsage(90, 4, 20, 10);
}

void Init_Display()
{
    tft.initR(INITR_MINI128x64);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextWrap(false);
    tft.setTextSize(2);

    mtm_Display.TaskRegister(0, Task_TFT_Display, 10);
    mtm_Display.TaskRegister(1, Task_ShowFPS, 1000, OFF);
    mtm_Display.TaskRegister(2, Task_CursorMoveUpdate, 10);
    mtm_Display.TaskRegister(3, Task_GUI_Wigdet_Update, 1000);
}

void Init_GUI()
{
    Draw_RGBBitmap_Logo(0.5, 0.5, 30);
    tft.clear();
    Draw_RGBBitmap_CtrlPage(0.25, 0.5, 0.75, 0.6);
}
