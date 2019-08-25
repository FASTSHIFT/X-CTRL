#ifndef __GUI_Private_H
#define __GUI_Private_H

#include "PageManager.h"
#include "GUI_Widget.h"
#include "Bitmap.h"

#define StatusBar_POS 12
#define clear() clearDisplay(BLACK)
#define clearPage() fillRect(0,StatusBar_POS+1,oled.width(),oled.height()-(StatusBar_POS+1),BLACK)

typedef enum {
    PAGE_MainMenu,
    PAGE_CtrlInfo,
    PAGE_Options,
    PAGE_SetJoystick,
    PAGE_SetBluetooth,
	PAGE_SetGravity,
    PAGE_About,
	PAGE_END
} PageNum_TypeDef;

#endif
