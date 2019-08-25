#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "PageManager.h"
#include "TFT_S6B33B2.h"

#define SCREEN_CLASS TFT_S6B33B2

#define ClearDisplay()      screen.fillScreen(screen.Black)

extern SCREEN_CLASS screen;
extern PageManager page;

typedef enum {
    PAGE_MAX
} PageNum_Type;

typedef enum {
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_ButtonRelease,
    EVENT_EncoderRotate,
    EVENT_MAX
} Event_Type;

#endif
