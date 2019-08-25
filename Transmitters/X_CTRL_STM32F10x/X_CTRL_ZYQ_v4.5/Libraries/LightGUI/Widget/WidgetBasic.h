#ifndef __LG_WIDGETBASIC_H
#define __LG_WIDGETBASIC_H

#include "stdint.h"

namespace LightGUI{

#define WIDGET_Default_BG T::Black//默认控件底色
#define WIDGET_Default_WG T::White//默认控件颜色

#ifndef ABS
#define ABS(x) (((x)>0)?(x):-(x))
#endif

/****** 控件基本类 ******/
template <class T> class WidgetBasic { 
public:
    WidgetBasic(T *obj, int16_t x, int16_t y, int16_t w, int16_t h);
    ~WidgetBasic();

    T *screen;//屏幕对象指针
    bool isDisplay;        //显示标志位
    int16_t X, Y;          //控件坐标
    int16_t Width, Height; //控件宽高
    int16_t X_Last, Y_Last;         //上一次控件坐标
    int16_t Width_Last, Height_Last;//上一次控件宽高
    uint16_t Color_BG;     //控件底色

    virtual void display(); //显示控件
    void noDisplay();       //不显示控件
    void setPosition(int16_t x, int16_t y); //设置控件坐标
    void setSize(int16_t w, int16_t h);     //设置控件大小
    void updatePosition();  //更新控件位置
private:   
};

/**
  * @brief  控件基本类构造函数
  * @param  obj: 引用屏幕对象
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @retval 无
  */
template <class T> WidgetBasic<T>::WidgetBasic(T *obj, int16_t x, int16_t y, int16_t w, int16_t h)
{
    screen = obj;
    X = X_Last = x;//设置默认参数
    Y = Y_Last = y;
    Width = Width_Last = w;
    Height = Height_Last = h;
    Color_BG = WIDGET_Default_BG;
    isDisplay = false;
}

/**
  * @brief  控件基本类析构函数
  * @param  无
  * @retval 无
  */
template <class T> WidgetBasic<T>::~WidgetBasic()
{
    noDisplay();
}

/**
  * @brief  显示控件
  * @param  无
  * @retval 无
  */
template <class T> void WidgetBasic<T>::display()
{
}

/**
  * @brief  不显示控件
  * @param  无
  * @retval 无
  */
template <class T> void WidgetBasic<T>::noDisplay()
{
    screen->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

/**
  * @brief  更新控件位置
  * @param  无
  * @param  无
  * @retval 无
  */
template <class T> void WidgetBasic<T>::updatePosition()
{
    if(X == X_Last && Y == Y_Last && isDisplay)return;

    if(!isDisplay)
        X_Last = X, Y_Last = Y;

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
    display();
    X_Last = X, Y_Last = Y; 
}

/**
  * @brief  设置控件坐标
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @retval 无
  */
template <class T> void WidgetBasic<T>::setPosition(int16_t x, int16_t y)
{
    X = x;
    Y = y;
    updatePosition();
}

/**
  * @brief  设置控件宽高
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @retval 无
  */
template <class T> void WidgetBasic<T>::setSize(int16_t w, int16_t h)
{
    Width = w;
    Height = h;
    if(Width == Width_Last && Height == Height_Last && isDisplay)//判断新宽高是否与旧宽高重复
        return;

    if(!isDisplay)
        Width_Last = Width, Height_Last = Height;

    screen->fillRect(X, Y, Width_Last, Height_Last, Color_BG);
    display();
    Width_Last = Width, Height_Last = Height;
}

}

#endif
