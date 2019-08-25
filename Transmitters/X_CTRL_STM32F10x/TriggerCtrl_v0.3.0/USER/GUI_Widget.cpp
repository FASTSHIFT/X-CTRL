#include "GUI_Widget.h"

//****************** WidgetBasic Class ******************//

WidgetBasic::WidgetBasic(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h)
    : tft(&obj)
{
    X = X_Last = x;
    Y = Y_Last = y;
    Width = Width_Last = w;
    Height = Height_Last = h;
    Color_BG = ST7735_BLACK;
}

WidgetBasic::~WidgetBasic()
{
    tft->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

void WidgetBasic::display()
{
}

void WidgetBasic::noDisplay()
{
    tft->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

void WidgetBasic::setPosition(int16_t x, int16_t y)
{
    X = x;
    Y = y;
    if(!isDisplay)X_Last = X, Y_Last = Y;
    if(X != X_Last || Y != Y_Last)
    {
        tft->fillRect(X_Last, Y_Last, Width, Height, Color_BG);
        X_Last = X, Y_Last = Y;
    }
    display();
}

void WidgetBasic::setSize(uint16_t w, uint16_t h)
{
    Width = w;
    Height = h;
    if(Width != Width_Last || Height != Height_Last)
    {
        tft->fillRect(X, Y, Width_Last, Height_Last, Color_BG);
        Width_Last = Width, Height_Last = Height;
    }
    display();
}

//****************** ProgressBar Class ******************//
ProgressBar::ProgressBar(WIDGET_TFT_OBJECT &obj, uint8_t dir, uint8_t style)
    : WidgetBasic(obj, 0, 0, 0, 0)
{
    Dir = dir;
    Style = style;
    Color_PB = ST7735_WHITE;
    Color_FM = ST7735_WHITE;
}

ProgressBar::ProgressBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t dir, uint8_t style)
    : WidgetBasic(obj, x, y, w, h)
{
    Dir = dir;
    Style = style;
    Color_PB = ST7735_WHITE;
    Color_FM = ST7735_WHITE;
}

void ProgressBar::display()
{
    tft->drawRect(X, Y, Width, Height, Color_FM);
    if(Dir == 0)
    {
        uint16_t w2 = (Width - 4) * (Progress / 1000.0);
        uint16_t w3 = Width - 4 - w2;
        int16_t x3 = X + 2 + w2;

        tft->fillRect(X + 2, Y + 2, w2, Height - 4, Color_PB);
        tft->fillRect(x3, Y + 2, w3, Height - 4, Color_BG);
    }
    else
    {
        uint16_t h2 = (Height - 4) * (Progress / 1000.0);
        uint16_t h3 = Height - 4 - h2;
        uint16_t y2 = Y + 2 + h3;

        tft->fillRect(X + 2, y2, Width - 4, h2, Color_PB);
        tft->fillRect(X + 2, Y + 2, Width - 4, h3, Color_BG);
    }
    isDisplay = true;
}

void ProgressBar::setProgress(float progress)
{
    progress = constrain(progress, 0.0, 1.0);
    Progress = progress * 1000.0;
    if(Progress != Progress_Last)
    {
        display();
        Progress_Last = Progress;
    }
    isDisplay = true;
}

#include "FileGroup.h"

void Draw_BattUsage(uint8_t BattUsage, int16_t x, int16_t y, int16_t w, int16_t h)
{
    static ProgressBar PB_Batt((WIDGET_TFT_OBJECT&)tft, 0);
    PB_Batt.X = x;
    PB_Batt.Y = y;
    PB_Batt.Width = w;
    PB_Batt.Height = h;

    if(BattUsage < 20)
    {
        PB_Batt.Color_PB = ST7735_RED;
    }
    else if(BattUsage >= 90)
    {
        PB_Batt.Color_PB = ST7735_GREEN;
    }
    else
    {
        PB_Batt.Color_PB = ST7735_WHITE;
    }

    PB_Batt.setProgress(BattUsage / 100.0);
    tft.drawFastVLine(x + w, y + 2, h - 4, ST7735_WHITE);
}


//****************** Cursor Class ******************//

Cursor::Cursor(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h)
    : WidgetBasic(obj, x, y, w, h)
{
    Color_CS = ST7735_WHITE;
    Style = 0;
    DisplayCallbackFunction = 0;
}

void Cursor::display()
{
    if(DisplayCallbackFunction)
    {
        DisplayCallbackFunction();
    }
    else
    {
        switch(Style)
        {
        case 0:
            tft->fillRect(X, Y, Width, Height, Color_CS);
            break;
        case 1:
            tft->drawRect(X, Y, Width, Height, Color_CS);
            break;
        }
    }
    isDisplay = true;
}

//****************** ScrollBar Class ******************//

ScrollBar::ScrollBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t dir, uint16_t l)
    : WidgetBasic(obj, x, y, w, h)
{
    Color_SB = ST7735_WHITE;
    Color_FM = ST7735_WHITE;
    Long = l;
    Dir = dir;
}

void ScrollBar::display()
{
    tft->drawRect(X, Y, Width, Height, Color_FM);

    if(Dir == 0)
    {
        uint16_t w1 = (Width - Long) * (Scroll / 1000.0);
        int16_t x1 = X + w1;
        int16_t x2 = x1 + Long;
        uint16_t w2 = Width - w1 - Long - 1;
        tft->fillRect(X + 1, Y + 1, w1, Height - 2, Color_BG);
        tft->fillRect(x1, Y + 1, Long, Height - 2, Color_SB);
        tft->fillRect(x2, Y + 1, w2, Height - 2, Color_BG);
    }
    else
    {
        uint16_t h1 = (Height - Long) * (1.0 - Scroll / 1000.0);
        int16_t y1 = Y + h1;
        int16_t y2 = y1 + Long;
        uint16_t h2 = Height - h1 - Long - 1;
        tft->fillRect(X + 1, Y + 1, Width - 2, h1, Color_BG);
        tft->fillRect(X + 1, y1, Width - 2, Long, Color_SB);
        tft->fillRect(X + 1, y2, Width - 2, h2, Color_BG);

    }
    isDisplay = true;
}

void ScrollBar::setScroll(float scroll)
{
    scroll = constrain(scroll, 0.0, 1.0);
    Scroll = scroll * 1000.0;
    if(Scroll != Scroll_Last)
    {
        display();
        Scroll_Last = Scroll;
    }
}

//****************** JoystickPos Class ******************//

JoystickPos::JoystickPos(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r)
    : WidgetBasic(obj, x, y, w, h)
{
    Color_FM = Color_JS = ST7735_WHITE;
    R = r;

    X_JsLast = X_Js = (2 * X + Width) / 2;
    Y_JsLast = Y_Js = (2 * Y + Height) / 2;
}

void JoystickPos::display()
{
    X_Js = fmap(Xc, -1.0, 1.0, X + R + 1, X + Width - R - 1);
    Y_Js = fmap(Yc, -1.0, 1.0, Y + R + 1, Y + Height - R - 1);
    tft->drawRect(X, Y, Width, Height, Color_FM);
    tft->fillCircle(X_JsLast, Y_JsLast, R, Color_BG);
    tft->fillCircle(X_Js, Y_Js, R, Color_JS);
    isDisplay = true;
}

void JoystickPos::setJsPos(float x, float y)
{
    x = constrain(x, -1.0, 1.0);
    y = constrain(y, -1.0, 1.0);
    Xc = x, Yc = -y;
    X_Js = fmap(Xc, -1.0, 1.0, X + R + 1, X + Width - R - 1);
    Y_Js = fmap(Yc, -1.0, 1.0, Y + R + 1, Y + Height - R - 1);
    if(X_Js != X_JsLast || Y_Js != Y_JsLast)
    {
        display();
        X_JsLast = X_Js, Y_JsLast = Y_Js;
    }
}
