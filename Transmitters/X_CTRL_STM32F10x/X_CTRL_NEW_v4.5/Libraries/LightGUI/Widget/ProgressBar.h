#ifndef __LG_PROGRESSBAR_H
#define __LG_PROGRESSBAR_H

#include "WidgetBasic.h"

namespace LightGUI {

/****** 进度条控件  ******/
/****** ProgressBar ******/
template <class T> class ProgressBar : public WidgetBasic<T> {
public:
    ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style = 0);
    ProgressBar(T *obj, uint8_t dir, uint8_t style = 0);

    uint16_t Dir, Progress, Style; ////方向(0:横向, 1:纵向), 进度, 类型
    uint16_t Color_PB, Color_FM;   //进度条颜色, 外框颜色

    virtual void display();
    void setProgress(float progress);//设置进度(0.0~1.0 -> 0~100%)
private:
    uint16_t Progress_Last;//上一次进度
};

/**
  * @brief  进度条构造函数
  * @param  obj: 引用屏幕对象
  * @param  dir: 方向(0:横向, 1:纵向)
  * @param  style:  类型
  * @retval 无
  */
template <class T> ProgressBar<T>::ProgressBar(T *obj, uint8_t dir, uint8_t style)
    : WidgetBasic<T>(obj, 0, 0, 0, 0)
{
    Dir = dir;
    Style = style;
    Color_PB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
}

/**
  * @brief  进度条构造函数
  * @param  obj: 引用屏幕对象
  * @param  x:   x坐标
  * @param  y:   y坐标
  * @param  w:   控件宽度
  * @param  h:   控件高度
  * @param  dir: 方向(0:横向, 1:纵向)
  * @param  style:  类型
  * @retval 无
  */
template <class T> ProgressBar<T>::ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Dir = dir;
    Style = style;
    Color_PB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
}

/**
  * @brief  显示进度条
  * @param  无
  * @retval 无
  */
template <class T> void ProgressBar<T>::display()
{
    this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_FM);
    if(Dir == 0)
    {
        int16_t w2 = (this->Width - 4) * (Progress / 1000.0);
        int16_t w3 = this->Width - 4 - w2;
        int16_t x3 = this->X + 2 + w2;

        if(w2 > 0)this->screen->fillRect(this->X + 2, this->Y + 2, w2, this->Height - 4, Color_PB);
        if(w3 > 0)this->screen->fillRect(x3, this->Y + 2, w3, this->Height - 4, this->Color_BG);
    }
    else
    {
        int16_t h2 = (this->Height - 4) * (Progress / 1000.0);
        int16_t h3 = this->Height - 4 - h2;
        int16_t y2 = this->Y + 2 + h3;

        if(h2 > 0)this->screen->fillRect(this->X + 2, y2, this->Width - 4, h2, this->Color_PB);
        if(h3 > 0)this->screen->fillRect(this->X + 2, this->Y + 2, this->Width - 4, h3, this->Color_BG);
    }
    this->isDisplay = true;
}

/**
  * @brief  设置进度条进度
  * @param progress: 进度(0.0~1.0 -> 0~100%)
  * @retval 无
  */
template <class T> void ProgressBar<T>::setProgress(float progress)
{
    progress = constrain(progress, 0.0, 1.0);
    Progress = progress * 1000.0;
    if(Progress == Progress_Last && this->isDisplay)return;
    
    display();
    Progress_Last = Progress;
}

}

#endif
