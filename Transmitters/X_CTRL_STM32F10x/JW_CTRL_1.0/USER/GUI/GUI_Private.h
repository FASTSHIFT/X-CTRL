#ifndef __GUI_PRIVATE_H
#define __GUI_PRIVATE_H

#include "Bitmaps/Bitmaps.h"
#include "LightGUI.h"
#include "Engine3D.h"
#include "PageManager.h"
#include "MenuManager.h"
#include "ButtonPrivate.h"
#include "Adafruit_SSD1306.h"

#define SCREEN_CLASS Adafruit_SSD1306
#define StatusBarPos 12
#define ClearPage() screen.fillRect(0,StatusBarPos+1,screen.width(),screen.height()-(StatusBarPos+1),screen.Black)

typedef enum{
    PAGE_MainMenu,
    PAGE_CtrlInfo,
    PAGE_Options,
    PAGE_SetJoystick,
    PAGE_About,
    PAGE_Handshake,
	PAGE_END
}PageNum_Type;

void PageRegister_MainMenu();
void PageRegister_CtrlInfo();
void PageRegister_Options();
void PageRegister_SetJoystick();
void PageRegister_About();
void PageRegister_Handshake();

extern SCREEN_CLASS screen;
extern PageManager page;

void PageDelay(uint32_t ms);
void Init_StatusBar();
void Thread_StatusBar();

#endif
