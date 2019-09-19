#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

#include "TFT_ILI9488.h"
#include "touch.h"

#define SCREEN_CLASS TFT_ILI9488
extern SCREEN_CLASS screen;

#define TEXT_HEIGHT_1   8
#define TEXT_WIDTH_1    6
#define TextSetDefault() screen.setTextColor(screen.White,screen.Black),screen.setTextSize(1)
#define TextMid(x,textnum) (screen.width()*(x)-(TEXT_WIDTH_1*(textnum))/2.0f)
#define TextMidPrint(x,y,text) screen.setCursor(TextMid(x,strlen(text)),screen.height()*(y)-TEXT_HEIGHT_1/2.0f),screen.print(text)
#define ScreenMid_W (screen.width()/2)
#define ScreenMid_H (screen.height()/2)

#include "lvgl.h"
#include "lv_conf.h"
#include "lv_ex_conf.h"

void lv_user_init();
void lv_user_fs_init();

void PageCreat_LuaScript();

#endif
