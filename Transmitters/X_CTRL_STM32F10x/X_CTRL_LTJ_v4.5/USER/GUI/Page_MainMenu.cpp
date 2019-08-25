#include "FileGroup.h"
#include "GUI_Private.h"

#define PosCtrlInfo (160*0.25-15)
#define PosOptions  (160*0.75-15)
#define PosCursorY  (80*0.859)

static MillisTaskManager mtm_MainMenu(2);
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Ctrl(&screen, (uint16_t*)gImage_Bitmap_Ctrl, 0, 0, 30, 30);
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Option(&screen, (uint16_t*)gImage_Bitmap_Option, 0, 0, 30, 30);
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCtrlInfo, PosCursorY, 30, 2);

float CursorPosNow = PosCtrlInfo;
float CursorPosLast = PosCtrlInfo;
uint8_t CursorSelect = 0;
float CursorTarget[2] = {PosCtrlInfo, PosOptions};
static uint32_t StayPageTimePoint;
static bool IsSpoke;

static void Task_CursorUpdate()
{
    CursorPosNow = CursorTarget[CursorSelect % 2];
    if((CursorPosNow - CursorPosLast) != 0)
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2;

    MenuCursor.setPosition(CursorPosLast, PosCursorY);
}

static void Setup()
{
    StayPageTimePoint = millis();
    IsSpoke = false;
    for(float i = 0.7; i < 1.0; i += 0.05)
    {
        Bitmap_Ctrl.setPosition_f(0.5 - 0.25 * i, 0.6);
        Bitmap_Option.setPosition_f(0.5 + 0.25 * i, 0.6);
        if((ecd != 0 || btEcd || btLEFT || btRIGHT))
        {
            i = 1.0;
            Bitmap_Ctrl.setPosition_f(0.25, 0.6);
            Bitmap_Option.setPosition_f(0.75, 0.6);
            break;
        }
    }

    for(float y = screen.height(); y > PosCursorY; y --)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        delay(10);
    }

    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 20);
}

#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15
static void Loop()
{
    if(!IsSpoke && millis() - StayPageTimePoint > 10000)
    {
        if(BattUsage < 30)
            XFS_Speak("警告：电池电量低。");
        
        XFS_Speak(
            "当前电池电压%0.1f伏，剩余电量%0.0f%%",
            BattVoltage,
            fmap(BattVoltage, BattEmptyVoltage, BattFullVoltage, 0.0, 100.0)
        );
        
        if(BattUsage < 20)
            XFS_Speak("请立即充电");
        else if(BattUsage < 30)
            XFS_Speak("请及时充电");
        
        IsSpoke = true;
    }
    mtm_MainMenu.Running(millis());
}

static void Exit()
{
    for(float y = PosCursorY; y < screen.height() + 1; y ++)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        PageDelay(10);
    }
}

static void ButtonPressEvent()
{
    if(btLEFT && CursorSelect > 0)CursorSelect--;
    if(btRIGHT && CursorSelect < 1)CursorSelect++;

    if((btEcd || btUP || btDOWN))
    {
        if((int)CursorPosNow == (int)PosCtrlInfo)
        {
            CursorPosLast = PosCtrlInfo;
            if(State_Handshake)
                page.PageChangeTo(PAGE_Handshake);
            else
                page.PageChangeTo(PAGE_CtrlInfo);
        }
        else if((int)CursorPosNow == (int)PosOptions)
        {
            CursorPosLast = PosOptions;
            page.PageChangeTo(PAGE_Options);
        }
    }

}

static void ButtonLongPressEvent()
{
}

static void EncoderEvent()
{
    if(ecd < 0 && CursorSelect > 0)
        CursorSelect--;
    
    if(ecd > 0 && CursorSelect < 1)
        CursorSelect++;
}

void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
