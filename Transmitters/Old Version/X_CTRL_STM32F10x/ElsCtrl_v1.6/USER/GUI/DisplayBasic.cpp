#include "FileGroup.h"
#include "GUI_Private.h"

SCREEN_CLASS screen(-1);
PageManager page(PAGE_MAX, EVENT_MAX);

void Task_ScreenDisplay()
{
    screen.display();
}

void Init_Display()
{
    __LoopExecute(Wire.begin(), 100);
    screen.begin(SSD1306_SWITCHCAPVCC);
    screen.setRotation(2);
    screen.setTextSize(1);
    screen.setTextColor(screen.WHITE, screen.Black);
    screen.display();
}

void Init_Pages()
{
    PageRegister_Calibration(PAGE_Calibration);
    PageRegister_Handshake(PAGE_Handshake);
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
	PageRegister_Dashboard(PAGE_Dashboard);
}

void Init_GUI()
{
    Init_StatusBar();
    Init_Pages();
    if(btDOWN)
        page.PageChangeTo(PAGE_Calibration);
    else
    { 
        if(btUP)
            page.PageChangeTo(PAGE_Handshake);
        else
            page.PageChangeTo(PAGE_CtrlInfo);
    }
}

void PageDelay(uint32_t ms)
{
    uint32_t Stop_TimePoint = millis() + ms;
    while(millis() < Stop_TimePoint)
        Thread_StatusBar();
}

void Thread_GUI()
{
    Thread_StatusBar();
    page.Running();
}
