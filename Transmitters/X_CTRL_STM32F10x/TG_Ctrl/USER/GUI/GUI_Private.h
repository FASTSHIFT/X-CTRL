#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "PageManager.h"
#include "HMI_Ctrl.h"

#define HMI_SERIAL Serial2

extern PageManager page;

typedef enum{
	PAGE_MainMenu,
	PAGE_MAX
}PageNum_Type;

typedef enum{
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_EncoderRotate,
    EVENT_MAX
}Event_Type;

#endif
