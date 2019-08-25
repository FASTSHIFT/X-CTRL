#include "FileGroup.h"
#include "GUI_Private.h"
#include "MenuManager.h"
#include "ComPrivate.h"

static MillisTaskManager mtm_Options(2);

#define ItemStartY (StatusBarPos+8)
#define ItemStartX 14

static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(&screen, screen.width() + 5, StatusBarPos + 6, 4, 43, 1, 10);
static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, ItemStartY, 8, 8);

static String StrModel[] = {"COMMON", "CAR-DS", "CAR-SS"};
static String StrSpeed[] = {"250Kbps", "1Mbps", "2Mbps"};

static int16_t ItemDispalyPos;
namespace ItemNum {
enum {
    Mode,
    PassBack,
    Handshake,
    Sound,
    SetJoystick,
    About,
    SaveExit,
    MAX
};
}

MenuManager menu(ItemNum::MAX, 4, &page);

static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(ItemStartX, ItemStartY + pos * 10, ItemStartX + 94, 10, screen.Black);
}

static void DrawBoolSwitch(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool sw)
{
    screen.drawRoundRect(x, y, w, h, r, screen.White);
    if(sw)
    {
        screen.fillRect(x + 1, y + 1, w - 2, h - 2, screen.Black);
        screen.fillCircle(x + w - 3 - r, (y + y + h) / 2, r, screen.White);
    }
    else
    {
        screen.fillRect(x + 1, y + 1, w - 2, h - 2, screen.Black);
        screen.fillCircle(x + 2 + r, (y + y + h) / 2, r, screen.White);
    }
}

static void DrawItemSwitch(uint8_t pos, bool sw, int16_t x = screen.width() - 35)
{
    DrawBoolSwitch(x, ItemStartY + pos * 10, 15, 9, 2, sw);
}

static void UpadateCursor()
{
    ItemDispalyPos = menu.ItemSelect - menu.ItemTop;
}

static void UpdateItemTop()
{
    menu.UpdateItemStrBuffer();
    Scroll.setScroll(1.0 - ((float)menu.ItemTop / (float)(menu.ItemNum_Max - menu.ItemNumDisplay_Max)));
    screen.setTextColor(screen.White, screen.Black);
    for(int16_t i = 0; i < menu.ItemNumDisplay_Max; i++)
    {
        ClearItemPos(i);
        screen.setCursor(ItemStartX, ItemStartY + i * 10);
        screen.print(menu.ItemStrBuffer[i]);

        int16_t nowItemNum = (menu.ItemTop + i) % menu.ItemNum_Max;
        if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
            DrawItemSwitch(i, *((bool*)menu.ItemList[nowItemNum].Data));
    }
}

static void UpdateItemPos(uint8_t pos)
{
    menu.UpdateItemStrBuffer();
    screen.setTextColor(screen.White, screen.Black);
    ClearItemPos(pos);
    screen.setCursor(ItemStartX, ItemStartY + pos * 10);
    screen.print(menu.ItemStrBuffer[pos]);

    int16_t nowItemNum = (menu.ItemTop + pos) % menu.ItemNum_Max;
    if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
        DrawItemSwitch(pos, *((bool*)menu.ItemList[nowItemNum].Data));
}

static void When_ItemSelect()
{
    switch(menu.ItemSelect)
    {
    case ItemNum::Mode:
        CTRL.Info.CtrlObject = ( CTRL.Info.CtrlObject + 1) % 3;
        menu.UpdateItem(ItemNum::Mode, ".Model:" + StrModel[CTRL.Info.CtrlObject]);
        break;
        
    case ItemNum::SaveExit:
        EEPROM_Handle(EEPROM_Chs::SaveData);
        break;
    }
}

static void Task_MenuUpdate()
{
    if(menu.ItemSelectUpdating)
    {
        UpadateCursor();
        menu.ItemSelectUpdating = false;
    }

    if(menu.ItemTopUpdating)
    {
        UpdateItemTop();
        menu.ItemTopUpdating = false;
    }

    if(menu.ItemXUpdating)
    {
        UpdateItemPos(ItemDispalyPos);
        menu.ItemXUpdating = false;
    }
}

static const float CursorTarget[4] = {ItemStartY, ItemStartY + 10, ItemStartY + 20, ItemStartY + 32};
static float CursorPosNow = ItemStartY;
static float CursorPosLast = ItemStartY + 30;

static void Task_DrawCursorUpdate()
{
    CursorPosNow = CursorTarget[ItemDispalyPos];
    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4;
    }

    ItemCursor.setPosition(ItemStartX - 8, CursorPosLast);
}

static void DrawCursor_Handler()
{
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setCursor(ItemCursor.X, ItemCursor.Y);
    screen.print('>');
}

static void Setup()
{
    ClearPage();
    CursorPosNow = ItemStartY;
    CursorPosLast = ItemStartY + 30;
    if(page.LastPage == PAGE_MainMenu)
    {
        ItemDispalyPos = menu.ItemSelect = menu.ItemTop = 0;
        menu.ItemXUpdating = menu.ItemSelectUpdating;
    }
    
    menu.ItemTopUpdating = true;

    menu.CallbackFunc_ItemSelectEvent = When_ItemSelect;
    menu.UpdateItem(ItemNum::Mode,    ".Model:"   + StrModel[CTRL.Info.CtrlObject]);
    menu.UpdateItem(ItemNum::PassBack,   ".PassBack",  menu.TYPE_Bool, (int)&State_PassBack);
    menu.UpdateItem(ItemNum::Handshake,  ".Handshake", menu.TYPE_Bool, (int)&State_Handshake);
    menu.UpdateItem(ItemNum::Sound,      ".Sound",     menu.TYPE_Bool, (int)&State_BuzzSound);
    menu.UpdateItem(ItemNum::SetJoystick,  ".Joystick   >",   menu.TYPE_PageJump, PAGE_SetJoystick);
    menu.UpdateItem(ItemNum::About,        ".About      >",   menu.TYPE_PageJump, PAGE_About);
    menu.UpdateItem(ItemNum::SaveExit,     ".Save & Exit",    menu.TYPE_PageJump, PAGE_MainMenu);

    screen.setTextColor(screen.White, screen.Black);
    menu.UpdateItemStrBuffer();
    for(uint8_t i = 0; i < menu.ItemNumDisplay_Max; i++)
    {
        for(int16_t j = -20; j <= ItemStartX - 6; j += 4)
        {
            screen.setCursor(j, ItemStartY + i * 10);
            screen.print(' ');
            screen.print(menu.ItemStrBuffer[i]);
        }
        BuzzTone(1000 + 100 * i, 5);
    }

    Scroll.setScroll(1.0);
    for(uint8_t i = screen.width() + 5; i > screen.width() - 7; i--)
    {
        Scroll.setPosition(i, Scroll.Y);
        PageDelay(10);
    }

    ItemCursor.DisplayCallbackFunction = DrawCursor_Handler;

    mtm_Options.TaskRegister(0, Task_MenuUpdate, 50);
    mtm_Options.TaskRegister(1, Task_DrawCursorUpdate, 20);
}

static void Loop()
{
    mtm_Options.Running(millis());
}

static void Exit()
{
    for(float i = 1.0; i > 0.0; i -= ((i * 0.01) + 0.001))
    {
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        PageDelay(1);
    }
    ClearPage();
}

static void ButtonPressEvent()
{
    if(btDOWN)
        menu.SelectMove(+1);
    if(btUP)
        menu.SelectMove(-1);

    if(btOK)
        menu.ItemSelectEvent();

    if(btBACK)
        page.PageChangeTo(PAGE_MainMenu);
}

static void ButtonLongPressEvent()
{
    if(btDOWN)
        menu.SelectMove(+1);
    if(btUP)
        menu.SelectMove(-1);
}

void PageRegister_Options()
{
#define ThisPage PAGE_Options
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
    page.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
