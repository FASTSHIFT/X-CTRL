/**
  ******************************************************************************
  * @file    LightGUI.h
  * @author  _VIFEXTECH
  * @version V1.5.0
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
  * 2019.2.18    V1.5 使用模版类重写，改进重绘函数，减少控件闪烁
  * 2019.2.19    V1.6 加入updatePosition()
  * 2019.3.19    V1.7 Bitmap贴图类已同时支持彩色图以及单色图
  * 2019.4.1     V1.8 添加布尔开关控件
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIGHTGUI_H
#define __LIGHTGUI_H

/* Includes ------------------------------------------------------------------*/
#include "Widget\Bitmap.h"
#include "Widget\BoolSwitch.h"
#include "Widget\Cursor.h"
#include "Widget\Joystick.h"
#include "Widget\ProgressBar.h"
#include "Widget\ScrollBar.h"

#endif
