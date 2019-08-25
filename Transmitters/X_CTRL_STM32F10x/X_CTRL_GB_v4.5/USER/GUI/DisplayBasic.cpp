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
	delay(200);
	__LoopExecute(Wire.begin(), 50);
    screen.begin(SSD1306_SWITCHCAPVCC);
    screen.setTextSize(1);
    screen.setTextColor(screen.White);
	screen.setRotation(2);
    screen.display();
}

void Init_Pages()
{
    PageRegister_MainMenu(PAGE_MainMenu);
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
}

void Init_GUI()
{
	TIM_Cmd(TIM3, ENABLE);
    Init_StatusBar();
    Init_Pages();
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
