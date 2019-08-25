#include "GUI_Widget.h"

//****************** WidgetBasic Class ******************//

WidgetBasic::WidgetBasic(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h)
    : tft(&obj)
{
    X = X_Last = x;
    Y = Y_Last = y;
    Width = Width_Last = w;
    Height = Height_Last = h;
    Color_BG = WIDGET_Default_BG;
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
    if(X == X_Last && Y == Y_Last)return;
    if(!isDisplay)X_Last = X, Y_Last = Y;
    if(X != X_Last || Y != Y_Last)
    {
        tft->fillRect(X_Last, Y_Last, Width, Height, Color_BG);
        X_Last = X, Y_Last = Y;
    }
    display();
}

void WidgetBasic::setSize(int16_t w, int16_t h)
{
    Width = w;
    Height = h;
    if(Width == Width_Last && Height == Height_Last)return;
    if(!isDisplay)Width_Last = Width, Height_Last = Height;
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
    Color_PB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
}

ProgressBar::ProgressBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style)
    : WidgetBasic(obj, x, y, w, h)
{
    Dir = dir;
    Style = style;
    Color_PB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
}

void ProgressBar::display()
{
    tft->drawRect(X, Y, Width, Height, Color_FM);
    if(Dir == 0)
    {
        int16_t w2 = (Width - 4) * (Progress / 1000.0);
        int16_t w3 = Width - 4 - w2;
        int16_t x3 = X + 2 + w2;

        if(w2 > 0)tft->fillRect(X + 2, Y + 2, w2, Height - 4, Color_PB);
        if(w3 > 0)tft->fillRect(x3, Y + 2, w3, Height - 4, Color_BG);
    }
    else
    {
        int16_t h2 = (Height - 4) * (Progress / 1000.0);
        int16_t h3 = Height - 4 - h2;
        int16_t y2 = Y + 2 + h3;

        if(h2 > 0)tft->fillRect(X + 2, y2, Width - 4, h2, Color_PB);
        if(h3 > 0)tft->fillRect(X + 2, Y + 2, Width - 4, h3, Color_BG);
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


//****************** Cursor Class ******************//

Cursor::Cursor(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h)
    : WidgetBasic(obj, x, y, w, h)
{
    Color_CS = WIDGET_Default_WG;
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
ScrollBar::ScrollBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint16_t l)
    : WidgetBasic(obj, x, y, w, h)
{
    Color_SB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
    Long = l;
    Dir = dir;
}

void ScrollBar::display()
{
    tft->drawRect(X, Y, Width, Height, Color_FM);

    if(Dir == 0)
    {
        int16_t w1 = (Width - Long - 1) * (Scroll / 1000.0) + 1;
        int16_t x1 = X + w1;
        int16_t x2 = x1 + Long;
        int16_t w2 = Width - w1 - Long - 1;
        if(w1 > 0)tft->fillRect(X + 1, Y + 1, w1, Height - 2, Color_BG);
        tft->fillRect(x1, Y + 1, Long, Height - 2, Color_SB);
        if(w2 > 0)tft->fillRect(x2, Y + 1, w2, Height - 2, Color_BG);
    }
    else
    {
        int16_t h1 = (Height - Long - 1) * (1.0 - Scroll / 1000.0) + 1;
        int16_t y1 = Y + h1;
        int16_t y2 = y1 + Long;
        int16_t h2 = Height - h1 - Long - 1;
        if(h1 > 0)tft->fillRect(X + 1, Y + 1, Width - 2, h1, Color_BG);
        tft->fillRect(X + 1, y1, Width - 2, Long, Color_SB);
        if(h2 > 0)tft->fillRect(X + 1, y2, Width - 2, h2, Color_BG);

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
JoystickPos::JoystickPos(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t r)
    : WidgetBasic(obj, x, y, w, h)
{
    Color_FM = Color_JS = WIDGET_Default_WG;
    R = r;

    X_JsLast = X_Js = (2 * X + Width) / 2;
    Y_JsLast = Y_Js = (2 * Y + Height) / 2;
}

void JoystickPos::display()
{
    X_Js = fmap(Xc, -1.0, 1.0, X + R + 1, X + Width - R - 1);
    Y_Js = fmap(Yc, -1.0, 1.0, Y + R + 1, Y + Height - R - 1);
    tft->fillCircle(X_JsLast, Y_JsLast, R, Color_BG);
    tft->fillCircle(X_Js, Y_Js, R, Color_JS);
    X_JsLast = X_Js, Y_JsLast = Y_Js;
    tft->drawRect(X, Y, Width, Height, Color_FM);
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
    }
}

//****************** Bitmap Class ******************//
Bitmap::Bitmap(WIDGET_TFT_OBJECT &obj, const uint8_t* bitmap_p, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    : tft(&obj)
{
    bitmap = bitmap_p;
    X = X_Last = x;
    Y = Y_Last = y;
    Width = w;
    Height = h;
    Color_BG = WIDGET_Default_BG;
    isDisplay = false;
}

void Bitmap::display()
{
    tft->drawBitmap(X, Y, bitmap, Width, Height, WIDGET_Default_WG, WIDGET_Default_BG);
    isDisplay = true;
}

void Bitmap::noDisplay()
{
    tft->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

void Bitmap::setPosition(int16_t x, int16_t y)
{
    X = x;
    Y = y;

    if(X == X_Last && Y == Y_Last)return;

    if(!isDisplay)X_Last = X, Y_Last = Y;

    if((abs(X - X_Last) > Width) || (abs(Y - Y_Last) > Height))
    {
        tft->fillRect(X_Last, X_Last, Width, Height, Color_BG);
    }
    else
    {
        if(X > X_Last)
        {
            tft->fillRect(X_Last, Y_Last, X - X_Last, Height, Color_BG);
        }
        else if(X < X_Last)
        {
            tft->fillRect(X + Width, Y_Last, X_Last - X, Height, Color_BG);
        }

        if(Y > Y_Last)
        {
            tft->fillRect(X_Last, X_Last, Width, Y - Y_Last, Color_BG);
        }
        else if(Y < Y_Last)
        {
            tft->fillRect(X_Last, Y + Height, Width, Y_Last - Y, Color_BG);
        }
    }
    X_Last = X, Y_Last = Y;
    display();
}

void Bitmap::setPosition_f(float x, float y)
{
    setPosition(tft->width() * x - Width / 2, tft->height() * y - Height / 2);
}
