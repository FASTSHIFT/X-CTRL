#ifndef __LG_BITMAP_H
#define __LG_BITMAP_H

#include "WidgetBasic.h"

namespace LightGUI {

/******  贴图控件   ******/
/******   Bitmap    ******/
template <class T> class Bitmap {
    typedef void(*FastDrawRGBBitmapFunction_t)(int16_t, int16_t, uint16_t*, int16_t, int16_t);
    typedef void(*FastDrawBitmapFunction_t)(int16_t, int16_t, uint8_t*, int16_t, int16_t, uint8_t, uint8_t);
public:
    Bitmap(T *obj, uint16_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h);
    Bitmap(T *obj, uint8_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h);
    uint16_t Color_BG;          //贴图背景色
    int16_t X, Y, Width, Height;//贴图坐标,长宽

    FastDrawRGBBitmapFunction_t FastDrawRGBBitmapCallback;
    FastDrawBitmapFunction_t    FastDrawBitmapCallback;
    void display();             //显示贴图
    void noDisplay();           //不显示贴图
    void setPosition(int16_t x, int16_t y);//设置贴图坐标
    void setPosition_f(float x, float y);//设置贴图屏幕映射坐标(0.0 ~ 1.0)
    void updatePosition();      //更新贴图位置
private:
    T *screen;  //屏幕对象指针
    uint16_t* rgbbitmap;           //指向RGB贴图的指针
    uint8_t* bitmap;           //指向RGB贴图的指针
    bool isDisplay;             //显示标志位
    int16_t X_Last, Y_Last;     //上一次控件坐标
};

/**
  * @brief  RGB贴图控件构造函数
  * @param  obj: 引用屏幕对象
  * @param  bitmap_p: 指向RGB贴图的指针
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @retval 无
  */
template <class T> Bitmap<T>::Bitmap(T *obj, uint16_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h)
{
    screen = obj;
    rgbbitmap = bitmap_p;
    bitmap = 0;
    X = X_Last = x;
    Y = Y_Last = y;
    Width = w;
    Height = h;
    Color_BG = WIDGET_Default_BG;
    FastDrawRGBBitmapCallback = 0;
    FastDrawBitmapCallback = 0;
    isDisplay = false;
}

/**
  * @brief  单色贴图控件构造函数
  * @param  obj: 引用屏幕对象
  * @param  bitmap_p: 指向贴图的指针
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @retval 无
  */
template <class T> Bitmap<T>::Bitmap(T *obj, uint8_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h)
{
    screen = obj;
    rgbbitmap = 0;
    bitmap = bitmap_p;
    X = X_Last = x;
    Y = Y_Last = y;
    Width = w;
    Height = h;
    Color_BG = WIDGET_Default_BG;
    FastDrawRGBBitmapCallback = 0;
    FastDrawBitmapCallback = 0;
    isDisplay = false;
}

/**
  * @brief  显示贴图
  * @param  无
  * @retval 无
  */
template <class T> void Bitmap<T>::display()
{
    if(bitmap)
    {
        if(FastDrawBitmapCallback)
            FastDrawBitmapCallback(X, Y, bitmap, Width, Height, !Color_BG, Color_BG);
        else
            screen->drawBitmap(X, Y, bitmap, Width, Height, !Color_BG, Color_BG);
    }
    else if(rgbbitmap)
    {
        if(FastDrawRGBBitmapCallback)
            FastDrawRGBBitmapCallback(X, Y, rgbbitmap, Width, Height);
        else
            screen->drawRGBBitmap(X, Y, rgbbitmap, Width, Height);
    }
    isDisplay = true;
}

/**
  * @brief  不显示贴图
  * @param  无
  * @retval 无
  */
template <class T> void Bitmap<T>::noDisplay()
{
    screen->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

/**
  * @brief  更新贴图位置
  * @param  无
  * @param  无
  * @retval 无
  */
template <class T> void Bitmap<T>::updatePosition()
{
    if(X == X_Last && Y == Y_Last && isDisplay)return;

    if(!isDisplay)X_Last = X, Y_Last = Y;

    if((ABS(X - X_Last) > Width) || (ABS(Y - Y_Last) > Height))
    {
        screen->fillRect(X_Last, Y_Last, Width, Height, Color_BG);
    }
    else
    {
        if(X > X_Last)
        {
            screen->fillRect(X_Last, Y_Last, X - X_Last, Height, Color_BG);
        }
        else if(X < X_Last)
        {
            screen->fillRect(X + Width, Y_Last, X_Last - X, Height, Color_BG);
        }

        if(Y > Y_Last)
        {
            screen->fillRect(X_Last, Y_Last, Width, Y - Y_Last, Color_BG);
        }
        else if(Y < Y_Last)
        {
            screen->fillRect(X_Last, Y + Height, Width, Y_Last - Y, Color_BG);
        }
    }
    X_Last = X, Y_Last = Y;
    display();
}

/**
  * @brief  设置贴图坐标
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @retval 无
  */
template <class T> void Bitmap<T>::setPosition(int16_t x, int16_t y)
{
    X = x;
    Y = y;
    updatePosition();
}

/**
  * @brief  设置贴图屏幕映射坐标
  * @param  x:   x坐标(0.0 ~ 1.0)
  * @param  y:   y坐标(0.0 ~ 1.0)
  * @retval 无
  */
template <class T> void Bitmap<T>::setPosition_f(float x, float y)
{
    setPosition(screen->width() * x - Width / 2, screen->height() * y - Height / 2);
}

}

#endif
