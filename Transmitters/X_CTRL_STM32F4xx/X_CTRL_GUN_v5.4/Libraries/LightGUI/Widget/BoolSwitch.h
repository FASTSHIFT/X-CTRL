#ifndef __LG_BOOLSWITCH_H
#define __LG_BOOLSWITCH_H

#include "WidgetBasic.h"

namespace LightGUI {

/******  布尔开关控件  ******/
/******   BoolSwitch   ******/
template <class T> class BoolSwitch : public WidgetBasic<T> {
public:
    BoolSwitch(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);

    uint16_t Color_SW;//开关颜色
    uint16_t Color_SW_BG;//开启时的开关底色
    uint16_t Color_FM;//外框颜色
    int16_t R;//按钮半径
    bool SwitchState, SwitchState_Last;
    virtual void display();
    void SetSwitch(bool sw);
private:
};

/**
  * @brief  布尔开关控件构造函数
  * @param  obj: 引用屏幕对象
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @param  r:   按钮半径
  * @retval 无
  */
template <class T> BoolSwitch<T>::BoolSwitch(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_FM = Color_SW = WIDGET_Default_WG;
    Color_SW_BG = WIDGET_Default_BG;
    R = r;
}

/**
  * @brief  显示布尔开关
  * @param  无
  * @retval 无
  */
template <class T> void BoolSwitch<T>::display()
{
    this->screen->drawRoundRect(this->X, this->Y, this->Width, this->Height, R, Color_FM);
    if(SwitchState)
    {
        this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, this->Height - 2, Color_SW_BG);
        this->screen->fillCircle(this->X + this->Width - 3 - R, (this->Y + this->Y + this->Height) / 2, R, Color_SW);
    }
    else
    {
        this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, this->Height - 2, this->Color_BG);
        this->screen->fillCircle(this->X + 2 + R, (this->Y + this->Y + this->Height) / 2, R, Color_SW);
    }
    this->isDisplay = true;
}

/**
  * @brief  设置布尔开关
  * @param  sw: true打开，false关闭
  * @retval 无
  */
template <class T> void BoolSwitch<T>::SetSwitch(bool sw)
{
    SwitchState = sw;
    if(SwitchState == SwitchState_Last && this->isDisplay) return;

    display();
    SwitchState_Last = SwitchState;
}

}

#endif
