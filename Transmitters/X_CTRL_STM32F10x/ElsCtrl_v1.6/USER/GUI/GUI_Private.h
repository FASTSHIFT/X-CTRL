#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "stdint.h"
#include "LightGUI.h"
#include "PageManager.h"
#include "Bitmaps/Bitmaps.h"
#include "Adafruit_SSD1306.h"

#define SCREEN_CLASS Adafruit_SSD1306
#define StatusBarPos 33
#define clear() fillScreen(screen.Black)
#define ClearPage() screen.fillRect(StatusBarPos+1,0,screen.width()-(StatusBarPos+1),screen.height(),screen.Black)
#define GetNextPostion(x,index,base) (pow(ABS((float)x),(float)index)+base)

extern Adafruit_SSD1306 screen;
extern PageManager page;

typedef enum{
	PAGE_Calibration,
    PAGE_CtrlInfo,
	PAGE_Dashboard,
    PAGE_Handshake,
	PAGE_MAX
}PageNum_Type;

typedef enum{
    EVENT_ButtonPress,
    EVENT_ButtonLongPress,
    EVENT_MAX
}Event_Type;

void Init_StatusBar();
void Thread_StatusBar();
void PageDelay(uint32_t ms);

void PageRegister_Calibration(uint8_t ThisPage);
void PageRegister_Handshake(uint8_t ThisPage);
void PageRegister_CtrlInfo(uint8_t ThisPage);
void PageRegister_Dashboard(uint8_t ThisPage);

#endif
