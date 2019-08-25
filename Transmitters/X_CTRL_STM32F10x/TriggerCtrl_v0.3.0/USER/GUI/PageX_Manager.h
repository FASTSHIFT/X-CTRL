#ifndef __PAGE_MANAGER_H
#define __PAGE_MANAGER_H

#include "stdint.h"

typedef enum{
	PAGE_MainMenu,
	PAGE_CtrlInfo,
}PageNum_TypeDef;

void PageChangeTo_x(PageNum_TypeDef PageNum);
void PageBottonPressEvent_x();
void PageBottonLongPressEvent_x();

void PageSetup_CtrlInfo();
void PageLoop_CtrlInfo();
void PageExit_CtrlInfo();
void PageBottonPressEvent_CtrlInfo();
void PageBottonLongPressEvent_CtrlInfo();

void PageSetup_MainMenu();
void PageLoop_MainMenu();
void PageExit_MainMenu();
void PageBottonPressEvent_MainMenu();
void PageBottonLongPressEvent_MainMenu();
#endif
