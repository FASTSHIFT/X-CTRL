#include "FileGroup.h"
#include "GUI_Private.h"

PageManager page(PAGE_END);
SCREEN_CLASS screen(-1);

void Init_Screen()
{
    __LoopExecute(Wire.begin(),50);
    screen.begin(SSD1306_SWITCHCAPVCC);
    screen.setRotation(2);
    screen.setTextSize(1);
    screen.setTextColor(screen.WHITE);
    screen.display();
}

void Task_ScreenUpdate()
{
    screen.display();
}

void Init_Pages()
{
    PageRegister_MainMenu();
    PageRegister_CtrlInfo();
    PageRegister_Options();
    PageRegister_SetJoystick();
    PageRegister_About();
    PageRegister_Handshake();
}

void Init_GUI()
{
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
