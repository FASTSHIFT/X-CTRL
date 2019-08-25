/**
  ******************************************************************************
  * @file    GUI_Widget.h
  * @author  _VIFEXTECH
  * @version V1.4.0
  * @date    2019-2-5
  * @brief   Designed by _VIFEXTECH, Base on Adafruit_GFX, 低内存消耗轻量级图形控件库
  ******************************************************************************
  * @attention
  * 本图形控件库基于Adafruit_GFX二次封装,可以提供动态控制坐标、尺寸、颜色等功能
  ******************************************************************************
  * @changelog
  * 2018.9.27    V1.0 拥有基本的控件显示，动态控制坐标、尺寸、颜色等功能
  * 2018.9.30    V1.1 修复图形移动运算错误的BUG
  * 2018.10.5    V1.2 添加Bitmap class，提升刷新效率
  * 2019.1.20    V1.3 整理代码，添加注释
  * 2019.2.5     V1.4 tft改为screen，添加快速绘制图像回调函数
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GUI_WIDGET_H
#define __GUI_WIDGET_H

#include "Adafruit_GFX.h"    //Adafruit_GFX图形库
#include "Adafruit_ST7735.h" //屏幕驱动库

#define WIDGET_TFT_OBJECT Adafruit_ST7735         //定义屏幕对象
#define WIDGET_Default_BG WIDGET_TFT_OBJECT::Black//默认控件底色
#define WIDGET_Default_WG WIDGET_TFT_OBJECT::White//默认控件颜色

/****** 控件基本类 ******/
class WidgetBasic { 
public:
    WidgetBasic(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h);
    ~WidgetBasic();

    WIDGET_TFT_OBJECT *screen;//屏幕对象指针
    bool isDisplay;        //显示标志位
    int16_t X, Y;          //控件坐标
    int16_t Width, Height; //控件宽高
	uint16_t Color_BG;     //控件底色

    virtual void display(); //显示控件
    void noDisplay();       //不显示控件
    void setPosition(int16_t x, int16_t y); //设置控件坐标
    void setSize(int16_t w, int16_t h);     //设置控件大小
private:

    int16_t X_Last, Y_Last;         //上一次控件坐标
    int16_t Width_Last, Height_Last;//上一次控件宽高
};

/****** 进度条控件  ******/
/****** ProgressBar ******/
class ProgressBar : public WidgetBasic {
public:
    ProgressBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style = 0);
    ProgressBar(WIDGET_TFT_OBJECT &obj, uint8_t dir, uint8_t style = 0);

    uint16_t Dir, Progress, Style; ////方向(0:横向, 1:纵向), 进度, 类型
    uint16_t Color_PB, Color_FM;   //进度条颜色, 外框颜色

    virtual void display();
    void noDisplay();
    void setProgress(float progress);//设置进度(0.0~1.0 -> 0~100%)
private:
    uint16_t Progress_Last;//上一次进度
};


/******  指针控件  ******/
/******   Cursor   ******/
class Cursor : public WidgetBasic {
    typedef void(*CursorCallbackFunction_t)(void);
public:
    Cursor(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h);

    uint16_t Color_CS;//指针颜色
	uint8_t Style;//指针类型
	CursorCallbackFunction_t DisplayCallbackFunction;//指针重绘回调函数

    virtual void display();
private:
};

/******  滚动条控件  ******/
/******  ScrollBar   ******/
class ScrollBar : public WidgetBasic {
public:
    ScrollBar(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint16_t l);

    uint16_t Color_SB, Color_FM;//滚动条颜色, 外框颜色
    int16_t Scroll, Long;//滚动进度, 滚动条长度
    uint8_t Dir;//方向(0:横向, 1:纵向)

    virtual void display();
    void setScroll(float scroll);//设置滚动条进度(0.0~1.0 -> 0~100%)
    inline void setProgress(float scroll) {
        setScroll(scroll);
    };
private:
    int16_t Scroll_Last;
};

/******   摇杆控件   ******/
/******  JoystickPos ******/
class JoystickPos : public WidgetBasic {
public:
    JoystickPos(WIDGET_TFT_OBJECT &obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t r);

    uint16_t Color_JS, Color_FM;//摇杆颜色, 外框颜色
    int16_t R;                  //摇杆球半径
    int16_t X_Js, Y_Js;         //摇杆坐标
    float Xc, Yc;               //摇杆映射坐标(-1.0 ~ 1.0)

    virtual void display();
    void setJsPos(float x, float y);//设置摇杆映射坐标(-1.0 ~ 1.0)
private:
    int16_t X_JsLast, Y_JsLast;  //上一次的摇杆坐标
};

/******  贴图控件   ******/
/******   Bitmap    ******/
class Bitmap {
    typedef void(*FastDrawRGBBitmapFunction_t)(int16_t,int16_t,uint16_t*,int16_t,int16_t);
public:
    Bitmap(WIDGET_TFT_OBJECT &obj, uint16_t* bitmap_p, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    uint16_t Color_BG;          //贴图背景色
	int16_t X, Y, Width, Height;//贴图坐标,长宽
    
    FastDrawRGBBitmapFunction_t FastDrawRGBBitmapCallback;
    void display();             //显示贴图
    void noDisplay();           //不显示贴图
    void setPosition(int16_t x, int16_t y);//设置贴图坐标
	void setPosition_f(float x, float y);//设置贴图屏幕映射坐标(0.0 ~ 1.0)

private:
    WIDGET_TFT_OBJECT *screen;  //屏幕对象指针
    uint16_t* bitmap;           //贴图指针
    bool isDisplay;             //显示标志位
    int16_t X_Last, Y_Last;     //上一次控件坐标
};

#endif
