#ifndef __LG_CURSOR_H
#define __LG_CURSOR_H

#include "WidgetBasic.h"

namespace LightGUI{

/******  光标控件  ******/
/******   Cursor   ******/
template <class T> class Cursor : public WidgetBasic<T> {
    typedef void(*CursorCallbackFunction_t)(void);
public:
    Cursor(T *obj, int16_t x, int16_t y, int16_t w, int16_t h);

    uint16_t Color_CS;//光标颜色
	uint8_t Style;//光标类型
	CursorCallbackFunction_t DisplayCallbackFunction;//光标重绘回调函数

    virtual void display();
private:
};

/**
  * @brief  指针控件构造函数
  * @param  obj: 引用屏幕对象
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @retval 无
  */
template <class T> Cursor<T>::Cursor(T *obj, int16_t x, int16_t y, int16_t w, int16_t h)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_CS = WIDGET_Default_WG;
    Style = 0;
    DisplayCallbackFunction = 0;
}

/**
  * @brief  显示光标
  * @param  无
  * @retval 无
  */
template <class T> void Cursor<T>::display()
{
    if(DisplayCallbackFunction)
        DisplayCallbackFunction();
    else
    {
        switch(Style)
        {
        case 0:
            this->screen->fillRect(this->X, this->Y, this->Width, this->Height, Color_CS);
            break;
        case 1:
            this->screen->drawRect(this->X_Last, this->Y_Last, this->Width_Last, this->Height_Last, this->Color_BG);
            this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_CS);
            break;
        }
    }
    this->isDisplay = true;
}

}

#endif
