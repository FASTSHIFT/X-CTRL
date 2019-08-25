#include "FileGroup.h"
#include "GUI_Private.h"

SCREEN_CLASS screen(-1);
PageManager page(PAGE_MAX, EVENT_MAX);
bool IS_ScreenBusy;

void Task_ScreenDisplay()
{
	IS_ScreenBusy = true;
	screen.display();
	IS_ScreenBusy = false;
}

void Init_Display()
{
    screen.begin(SSD1306_SWITCHCAPVCC);
    screen.setTextSize(1);
    screen.setTextColor(screen.White);
    screen.display();
}

void Init_Pages()
{
    PageRegister_MainMenu(PAGE_MainMenu);
    PageRegister_CtrlInfo(PAGE_CtrlInfo);
    PageRegister_Options(PAGE_Options);
    PageRegister_SetJoystick(PAGE_SetJoystick);
    PageRegister_SetBluetooth(PAGE_SetBluetooth);
    PageRegister_SetGravity(PAGE_SetGravity);
    PageRegister_About(PAGE_About);
    PageRegister_Handshake(PAGE_Handshake);
}

void Init_GUI()
{
    if(Bluetooth_ConnectObject == BC_Type::BC_HMI)
        Init_HMI();
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
