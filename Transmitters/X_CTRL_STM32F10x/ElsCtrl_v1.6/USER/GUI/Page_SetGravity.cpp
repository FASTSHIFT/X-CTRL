#include "FileGroup.h"
#include "GUI_Private.h"

static MillisTaskManager mtm_SetGravity(2);
int16_t *from_MPU_LX = 0, *from_MPU_LY = 0, *from_MPU_RX = &MPU_Data.X, *from_MPU_RY = 0;

#define FM_Size 40

static LightGUI::Joystick<SCREEN_CLASS> MPU_Pos(&screen, (screen.width() - FM_Size) / 2, StatusBar_POS + 4, FM_Size, FM_Size, 4);

static void Task_UpdateMPUPos()
{
    MPU_Pos.setJsPos((float)MPU_Data.X / CtrlOutput_MaxValue, (float)MPU_Data.Y / CtrlOutput_MaxValue);
}

static void Task_UpdateMPU_State()
{
    extern float Pitch, Roll;
    screen.setCursor(5, StatusBar_POS + 8);
    screen.setTextSize(1);
    if(State_MPU)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("ON ");
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("OFF");
    }

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(5, StatusBar_POS + 2 * 8);
    screen.print("GX->");
    screen.setCursor(5, StatusBar_POS + 3 * 8);
    if(&MPU_Data.X == from_MPU_LX)screen.print("JsLX");
    else if(&MPU_Data.X == from_MPU_RX)screen.print("JsRX");
    else screen.print("None");

    screen.setCursor(5, StatusBar_POS + 4 * 8);
    screen.print("GY->");
    screen.setCursor(5, StatusBar_POS + 5 * 8);
    if(&MPU_Data.Y == from_MPU_LY)screen.print("JsLY");
    else if(&MPU_Data.Y == from_MPU_RY)screen.print("JsRY");
    else screen.print("None");

    screen.setCursor(87, StatusBar_POS + 8);
    screen.print("Pitch");
    screen.setCursor(87, StatusBar_POS + 2 * 8);
    screen.printf("%.2f ", Pitch);

    screen.setCursor(87, StatusBar_POS + 3.5 * 8);
    screen.print("Roll");
    screen.setCursor(87, StatusBar_POS + 4.5 * 8);
    screen.printf("%.2f ", Roll);
}

static void Setup()
{
    int16_t target = StatusBar_POS + 4;
    for(int16_t i = screen.height(); i > target; i--)
    {
        MPU_Pos.setPosition(MPU_Pos.X, i);
        MPU_Pos.setJsPos((float)MPU_Data.X / CtrlOutput_MaxValue, (float)MPU_Data.Y / CtrlOutput_MaxValue);
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
    for(int16_t i = MPU_Pos.Y; i < screen.height(); i++)
    {
        MPU_Pos.setPosition(MPU_Pos.X, i);
        MPU_Pos.setJsPos((float)MPU_Data.X / CtrlOutput_MaxValue, (float)MPU_Data.Y / CtrlOutput_MaxValue);
    }
}

static void ButtonPressEvent()
{
    if(btOK)State_MPU = !State_MPU;
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
    if(btBACK)page.PageChangeTo(PAGE_Options);
}

void PageRegister_SetGravity()
{
#define ThisPage PAGE_SetGravity
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
//	page.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
//	page.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
