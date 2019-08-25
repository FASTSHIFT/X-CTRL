#include "FileGroup.h"
#include "GUI_Private.h"

#define PosCtrlInfo (oled.width()*0.25-15)
#define PosOptions  (oled.width()*0.75-15)
#define PosCursorY 55

MillisTaskManager mtm_MainMenu(2);
static Bitmap Bitmap_Ctrl(oled, gImage_Bitmap_Ctrl, 0, 0, 30, 32);
static Bitmap Bitmap_Option(oled, gImage_Bitmap_Option, 0, 0, 30, 32);
static Cursor MenuCursor(oled, PosCtrlInfo, PosCursorY, 30, 2);

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
static const float CursorTarget[2] = {PosCtrlInfo, PosOptions};

static void Task_CursorUpdate()
{
    CursorPosNow = CursorTarget[CursorSelect % 2];
    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2;
    }

    MenuCursor.setPosition(CursorPosLast, PosCursorY);
}

static void Setup()
{
	oled.clearPage();
	
    for(float i = 0.7; i < 1.0; i += 0.03)
    {
        Bitmap_Ctrl.setPosition_f(0.5 - 0.25 * i, 0.6);
        Bitmap_Option.setPosition_f(0.5 + 0.25 * i, 0.6);
    }

    for(float y = oled.height(); y > PosCursorY; y --)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        delay(10);
    }

    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 20);
}

static void Loop()
{
    mtm_MainMenu.Running(millis());
}

static void Exit()
{
    for(float y = PosCursorY; y < oled.height() + 1; y ++)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        delay(5);
    }
}

static void ButtonPressEvent()
{
#define btLEFT btBACK
#define btRIGHT btOK
    if(btLEFT && CursorSelect > 0)CursorSelect--;
    if(btRIGHT && CursorSelect < 1)CursorSelect++;

    if(btUP || btDOWN)
    {
        if(CursorPosNow == PosCtrlInfo)
        {
            CursorPosLast = PosCtrlInfo;
            pm.PageChangeTo(PAGE_CtrlInfo);
        }
        else if(CursorPosNow == PosOptions)
        {
            CursorPosLast = PosOptions;
            pm.PageChangeTo(PAGE_Options);
        }
    }

}

static void ButtonLongPressEvent()
{

}

void PageRegister_MainMenu()
{
#define ThisPage PAGE_MainMenu
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
    pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
