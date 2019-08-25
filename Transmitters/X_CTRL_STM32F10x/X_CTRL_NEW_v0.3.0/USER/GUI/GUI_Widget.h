#ifndef __GUI_WIDGET_H
#define __GUI_WIDGET_H

#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"

class ProgressBar {
public:
    ProgressBar(Adafruit_ST7735 &obj, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t dir);
    ProgressBar(Adafruit_ST7735 &obj, uint8_t dir);
	~ProgressBar();

    uint16_t X, Y, Width, Height, Dir, Progress;
    uint16_t Color_PB, Color_BG, Color_FM;

    void display();
    void noDisplay();
    void setProgress(float progress);
    void setPosition(uint16_t x, uint16_t y);
    void setSize(uint16_t W, uint16_t h);
private:
    Adafruit_ST7735 *tft;
    bool isDisplay;
	uint16_t X_Last, Y_Last, Width_Last, Height_Last, Progress_Last;
};

#endif
