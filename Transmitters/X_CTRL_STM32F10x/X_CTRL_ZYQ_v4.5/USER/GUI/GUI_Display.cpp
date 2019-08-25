#include "FileGroup.h"
#include "GUI_Private.h"

Adafruit_SSD1306 oled(-1);
PageManager pm(PAGE_END);

void Init_GUI()
{	
	if(HMI_State) Init_HMI();
	TIM_Cmd(TaskManager_Timer, ENABLE);
	oled.clearDisplay();
	Init_StatusBar();
	
	PageRegister_MainMenu();
    PageRegister_CtrlInfo();
    PageRegister_Options();
    PageRegister_SetJoystick();
    PageRegister_SetBluetooth();
    PageRegister_SetGravity();
    PageRegister_About();
}

void Task_OledDisplay()
{
	oled.display();
}

