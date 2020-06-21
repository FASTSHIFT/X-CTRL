#ifndef __LG_JOYSTICK_H
#define __LG_JOYSTICK_H

#include "WidgetBasic.h"

namespace LightGUI {

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

/******   摇杆控件   ******/
/******  Joystick ******/
template <class T> class Joystick : public WidgetBasic<T> {
public:
    Joystick(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t r);

    uint16_t Color_JS, Color_FM;//摇杆颜色, 外框颜色
    int16_t R;                  //摇杆球半径
    int16_t X_Js, Y_Js;         //摇杆球坐标
    float Xc, Yc;               //摇杆球映射坐标(-1.0 ~ 1.0)

    virtual void display();
    void setJsPos(float x, float y);//设置摇杆映射坐标(-1.0 ~ 1.0)
private:
    int16_t X_JsLast, Y_JsLast;  //上一次的摇杆坐标
    bool UpdateAll;
    void UpdateJs();
};

/**
  * @brief  摇杆控件构造函数
  * @param  obj: 引用屏幕对象
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @param  r:   摇杆球半径
  * @retval 无
  */
template <class T> Joystick<T>::Joystick(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t r)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_FM = Color_JS = WIDGET_Default_WG;
    R = r;

    X_JsLast = X_Js = (2 * this->X + this->Width) / 2;
    Y_JsLast = Y_Js = (2 * this->Y + this->Height) / 2;
    UpdateAll = false;
}

/**
  * @brief  更新摇杆球坐标
  * @param  无
  * @retval 无
  */
template <class T> void Joystick<T>::UpdateJs()
{
    X_Js = fmap(Xc, -1.0, 1.0, this->X + R + 1, this->X + this->Width - R - 2);
    Y_Js = fmap(Yc, -1.0, 1.0, this->Y + R + 1, this->Y + this->Height - R - 2);
}

/**
  * @brief  显示摇杆
  * @param  无
  * @retval 无
  */
template <class T> void Joystick<T>::display()
{ 
    if(UpdateAll)
    {
        UpdateJs();
        this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_FM);
        this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, this->Height - 2, this->Color_BG); 
    }
    else
        this->screen->fillCircle(X_JsLast, Y_JsLast, R, this->Color_BG);
    
    this->screen->fillCircle(X_Js, Y_Js, R, Color_JS);
    X_JsLast = X_Js, Y_JsLast = Y_Js;
    
    this->isDisplay = true;
    UpdateAll = true;
}

/**
  * @brief  设置摇杆坐标
  * @param  x:   x坐标(-1.0~1.0)
  * @param  y:   y坐标(-1.0~1.0)
  * @retval 无
  */
template <class T> void Joystick<T>::setJsPos(float x, float y)
{
    x = constrain(x, -1.0, 1.0);
    y = constrain(y, -1.0, 1.0);
    Xc = x, Yc = -y;
    UpdateJs();
    if(X_Js == X_JsLast && Y_Js == Y_JsLast) return;
    
    UpdateAll = false;
    display();
}

}

#endif
