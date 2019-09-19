#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "GT911.h"   //增加GT911触摸IC

#define TP_PRES_DOWN 0x80  //触屏被按下   
#define TP_CATH_PRES 0x40  //有按键按下了     
#define CT_MAX_TOUCH  5    //电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct
{
    uint8_t (*init)(void);           //初始化触摸屏控制器
    uint8_t (*scan)(void);               //扫描触摸屏.0,屏幕扫描;1,物理坐标;
    uint16_t x[CT_MAX_TOUCH];        //当前坐标
    uint16_t y[CT_MAX_TOUCH];        //电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
    //x[4],y[4]存储第一次按下时的坐标.
    uint8_t  sta;                    //笔的状态
    //b7:按下1/松开0;
    //b6:0,没有按键按下;1,有按键按下.
    //b5:保留
    //b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
} Touch_TypeDef;

extern Touch_TypeDef tp_dev;        //触屏控制器在touch.c里面定义

uint8_t TP_Init(void);                               //初始化

#endif

















