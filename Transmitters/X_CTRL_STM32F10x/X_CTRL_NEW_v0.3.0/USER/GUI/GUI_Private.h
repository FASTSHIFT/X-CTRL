#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "Bezier.h"

extern BezierLine AnimationBezierLine;

void Draw_BattUsage(int16_t x, int16_t y, int16_t w, int16_t h);

void Draw_RGBBitmap_BasicPage(float pos_x1, float pos_x2, float pos_y);
void Draw_RGBBitmap_CtrlPage(float pos_x1, float pos_x2, float pos_x3, float pos_y);
void Draw_RGBBitmap_Logo(float x, float y, uint16_t time);

#endif
