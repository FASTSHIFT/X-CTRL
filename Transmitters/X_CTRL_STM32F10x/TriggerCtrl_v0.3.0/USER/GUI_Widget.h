#ifndef __GUI_WIDGET_H
#define __GUI_WIDGET_H

#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"

#define WIDGET_TFT_OBJECT Adafruit_ST7735

class WidgetBasic {
public:
    WidgetBasic(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h);
    ~WidgetBasic();

    WIDGET_TFT_OBJECT *tft;
    bool isDisplay;
    int16_t X, Y;
    uint16_t Width, Height, Color_BG;

    virtual void display();
    void noDisplay();
    void setPosition(int16_t x, int16_t y);
    void setSize(uint16_t w, uint16_t h);
private:

    int16_t X_Last, Y_Last;
    uint16_t Width_Last, Height_Last;
};

//****************** ProgressBar Class ******************//
class ProgressBar : public WidgetBasic {
public:
    ProgressBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t dir, uint8_t style = 0);
    ProgressBar(WIDGET_TFT_OBJECT &obj, uint8_t dir, uint8_t style = 0);

    uint16_t Dir, Progress, Style;
    uint16_t Color_PB, Color_FM;

    virtual void display();
    void noDisplay();
    void setProgress(float progress);
private:
    uint16_t Progress_Last;
};
void Draw_BattUsage(int16_t x, int16_t y, int16_t w, int16_t h);


//****************** Cursor Class ******************//
class Cursor : public WidgetBasic {
public:
    Cursor(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h);

    uint16_t Color_CS;
    uint8_t Style;
    CallbackFunction_t DisplayCallbackFunction;

    virtual void display();
private:
};

//****************** ScrollBar Class ******************//
class ScrollBar : public WidgetBasic {
public:
    ScrollBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t dir, uint16_t l);

    uint16_t Color_SB, Color_FM;
    uint16_t Scroll, Long;
    uint8_t Dir;

    virtual void display();
    void setScroll(float scroll);
    inline void setProgress(float scroll) {
        setScroll(scroll);
    };
private:
    uint16_t Scroll_Last;
};

//****************** JoystickPos Class ******************//
class JoystickPos : public WidgetBasic {
public:
    JoystickPos(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r);

    uint16_t Color_JS, Color_FM;
    uint16_t R;
    int16_t X_Js, Y_Js;
    float Xc, Yc;

    virtual void display();
    void setJsPos(float x, float y);
private:
    int16_t X_JsLast, Y_JsLast;
};

#endif
