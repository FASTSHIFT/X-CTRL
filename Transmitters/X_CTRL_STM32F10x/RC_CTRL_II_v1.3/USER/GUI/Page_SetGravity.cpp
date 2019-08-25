#include "FileGroup.h"
#include "GUI_Private.h"

MillisTaskManager mtm_SetGravity(2);
int16_t *from_MPU_LX = 0, *from_MPU_LY = 0, *from_MPU_RX = &MPU_Data.X, *from_MPU_RY = 0;

#define FM_Size 40

static JoystickPos MPU_Pos(oled, (oled.width() - FM_Size) / 2, StatusBar_POS + 4, FM_Size, FM_Size, 4);

static void Task_UpdateMPUPos()
{
    MPU_Pos.setJsPos((float)MPU_Data.X / CtrlOutput_MaxValue, (float)MPU_Data.Y / CtrlOutput_MaxValue);
}

static void Task_UpdateMPU_State()
{
    extern float Pitch, Roll;
    oled.setCursor(5, StatusBar_POS + 8);
	oled.setTextColor(WHITE,BLACK);
    if(MPU_State)
    {
        oled.print("ON ");
    }
    else
    {
        oled.print("OFF");
    }
	
	oled.setCursor(5, StatusBar_POS + 2*8);
	oled.print("GX->");
	oled.setCursor(5, StatusBar_POS + 3*8);
	if(&MPU_Data.X == from_MPU_LX)oled.print("JsLX");
	else if(&MPU_Data.X == from_MPU_RX)oled.print("JsRX");
	else oled.print("None");
	
	oled.setCursor(5, StatusBar_POS + 4*8);
	oled.print("GY->");
	oled.setCursor(5, StatusBar_POS + 5*8);
	if(&MPU_Data.Y == from_MPU_LY)oled.print("JsLY");
	else if(&MPU_Data.Y == from_MPU_RY)oled.print("JsRY");
	else oled.print("None");

    oled.setCursor(87, StatusBar_POS + 8);
	oled.print("Pitch");
	oled.setCursor(87, StatusBar_POS + 2*8);
    oled.printf("%.2f ", Pitch);
	
	oled.setCursor(87, StatusBar_POS + 3.5*8);
	oled.print("Roll");
    oled.setCursor(87, StatusBar_POS + 4.5*8);
    oled.printf("%.2f ", Roll);
}

static void Setup()
{
    int16_t target = StatusBar_POS + 4;
    for(int16_t i = oled.height(); i > target; i--)
    {
        MPU_Pos.setPosition(MPU_Pos.X, i);
        MPU_Pos.setJsPos((float)MPU_Data.X / CtrlOutput_MaxValue, (float)MPU_Data.Y / CtrlOutput_MaxValue);
		delay(1);
    }

    mtm_SetGravity.TaskRegister(0, Task_UpdateMPUPos, 10);
    mtm_SetGravity.TaskRegister(1, Task_UpdateMPU_State, 50);
}

static void Loop()
{
    mtm_SetGravity.Running(millis());
}

static void Exit()
{
    for(int16_t i = MPU_Pos.Y; i < oled.height(); i++)
    {
        MPU_Pos.setPosition(MPU_Pos.X, i);
        MPU_Pos.setJsPos((float)MPU_Data.X / CtrlOutput_MaxValue, (float)MPU_Data.Y / CtrlOutput_MaxValue);
		delay(1);
    }
}

static void ButtonPressEvent()
{
    if(btOK)MPU_State = !MPU_State;
	if(btUP)
	{
		if(from_MPU_LX == 0 && from_MPU_RX == 0)
		{
			from_MPU_LX = &MPU_Data.X;
			from_MPU_RX = 0;
		}
		else if(from_MPU_LX != 0 && from_MPU_RX == 0)
		{
			from_MPU_LX = 0;
			from_MPU_RX = &MPU_Data.X;
		}
		else
		{
			from_MPU_LX = from_MPU_RX = 0;
		}
	}
	if(btDOWN)
	{
		if(from_MPU_LY == 0 && from_MPU_RY == 0)
		{
			from_MPU_LY = &MPU_Data.Y;
			from_MPU_RY = 0;
		}
		else if(from_MPU_LY != 0 && from_MPU_RY == 0)
		{
			from_MPU_LY = 0;
			from_MPU_RY = &MPU_Data.Y;
		}
		else
		{
			from_MPU_LY = from_MPU_RY = 0;
		}
	}
	
    if(btBACK)pm.PageChangeTo(PAGE_Options);
}

void PageRegister_SetGravity()
{
#define ThisPage PAGE_SetGravity
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
//	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
//	pm.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
