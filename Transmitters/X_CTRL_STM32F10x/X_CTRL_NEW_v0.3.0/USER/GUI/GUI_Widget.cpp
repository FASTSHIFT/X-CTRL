#include "GUI_Widget.h"

ProgressBar::ProgressBar(Adafruit_ST7735 &obj, uint8_t dir)
    : tft(&obj)
{
    Dir = dir;
    Color_PB = ST7735_WHITE;
    Color_BG = ST7735_BLACK;
    Color_FM = ST7735_WHITE;
    isDisplay = false;
}

ProgressBar::ProgressBar(Adafruit_ST7735 &obj, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t dir)
    : tft(&obj)
{
    X = x;
    Y = y;
    Width = w;
    Height = h;
    Dir = dir;
    Color_PB = ST7735_WHITE;
    Color_BG = ST7735_BLACK;
    Color_FM = ST7735_WHITE;
    isDisplay = false;
}

ProgressBar::~ProgressBar()
{
    tft->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

void ProgressBar::display()
{
    tft->drawRect(X, Y, Width, Height, Color_FM);
    tft->fillRect(X + 2, Y + 2, (Width - 6), Height - 4, Color_BG);
    tft->fillRect(X + 2, Y + 2, (Width - 6) * (Progress / 1000.0), Height - 4, Color_PB);
    isDisplay = true;
}

void ProgressBar::noDisplay()
{
    tft->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

void ProgressBar::setProgress(float progress)
{
    Progress = progress * 1000.0;
    if(!isDisplay)
    {
        tft->drawRect(X, Y, Width, Height, Color_FM);
    }
    tft->fillRect(X + 2, Y + 2, (Width - 4), Height - 4, Color_BG);
    tft->fillRect(X + 2, Y + 2, (Width - 4) * progress, Height - 4, Color_PB);
    isDisplay = true;
}

void ProgressBar::setPosition(uint16_t x, uint16_t y)
{
    X = x;
    Y = y;
    if(X != X_Last || Y != Y_Last)
    {
        tft->fillRect(X_Last, Y_Last, Width, Height, Color_BG);
        X_Last = X, Y_Last = Y;
    }
    display();
}

void ProgressBar::setSize(uint16_t w, uint16_t h)
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


#include "FileGroup.h"
#include "GUI_Private.h"

void Draw_BattUsage(int16_t x, int16_t y, int16_t w, int16_t h)
{
    static ProgressBar PB_Batt((Adafruit_ST7735&)tft, 0);
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
