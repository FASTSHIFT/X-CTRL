#include "FileGroup.h"
#include "GUI_Private.h"
#include "MenuManager.h"
#include "ComPrivate.h"

static MillisTaskManager mtm_Options(2);

#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14

#define TextHeight 10

static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, ItemStartY, 8, 8);
//static LightGUI::BoolSwitch<SCREEN_CLASS> Switch(&screen, screen.width() - 35, ItemStartY + 1 * 10, 15, 9, 2);

static String StrModel[] = {"COMMON", "CAR-DS", "CAR-SS"};
static String StrBtc[] = {"HMI", "XFS"};
static String StrSpeed[] = {"250Kbps", "1Mbps", "2Mbps"};

static int16_t ItemDispalyPos;
namespace ItemNum {
enum {
    Mode,
    Btc,
    Addr,
    Freq,
    Speed,
    PassBack,
    Handshake,
    Sound,
    Bluetooth,
    PowerSaving,
    SetJoystick,
    SetBlueTooth,
    SetGravity,
    About,
    SaveExit,
    MAX
};
}

MenuManager menu(ItemNum::MAX, 6, &page);

static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(
    &screen, 
    screen.width() + 5, 
    StatusBar_POS + 6, 
    4,
    menu.ItemNumDisplay_Max * TextHeight+1,
    1,
    menu.ItemNumDisplay_Max * TextHeight * 0.3
);

static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(ItemStartX, ItemStartY + pos * TextHeight, ItemStartX + 113, TextHeight, screen.Black);
}

static void DrawBoolSwitch(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool sw)
{
    screen.drawRoundRect(x, y, w, h, r, screen.White);
    if(sw)
    {
        screen.fillRect(x + 1, y + 1, w - 2, h - 2, screen.Green);
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
    DrawBoolSwitch(x, ItemStartY + pos * TextHeight, 15, 9, 2, sw);
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
        screen.setCursor(ItemStartX, ItemStartY + i * TextHeight);
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
    screen.setCursor(ItemStartX, ItemStartY + pos * TextHeight);
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

    case ItemNum::Btc:
        Bluetooth_ConnectObject++;
        if(Bluetooth_ConnectObject == BC_Type::BC_END)Bluetooth_ConnectObject = 0;
        menu.UpdateItem(ItemNum::Btc, ".Btc:" + StrBtc[Bluetooth_ConnectObject]);
        break;

    case ItemNum::Addr:
        NRF_Cfg.Address ++;
        if(NRF_Cfg.Address >= (sizeof(NRF_AddressConfig) / 5))
            NRF_Cfg.Address = 0;
        menu.UpdateItem(ItemNum::Addr, ".Address:" + String(NRF_Cfg.Address));
        nrf.SetAddress(NRF_AddressConfig + NRF_Cfg.Address * 5);
        break;

    case ItemNum::Freq:

        if(State_Handshake)
        {
            BuzzTone(100, 20);
            break;
        }

        NRF_Cfg.Freq += 10;
        if(NRF_Cfg.Freq > 120)NRF_Cfg.Freq = 0;
        menu.UpdateItem(ItemNum::Freq, ".Freq:" + String(2400 + NRF_Cfg.Freq) + "MHz");
        nrf.SetFreqency(NRF_Cfg.Freq);
        break;

    case ItemNum::Speed:

        if(State_Handshake)
        {
            BuzzTone(100, 20);
            break;
        }

        NRF_Cfg.Speed ++;
        NRF_Cfg.Speed %= 3;
        menu.UpdateItem(ItemNum::Speed, ".Speed:" + StrSpeed[NRF_Cfg.Speed]);
        if(NRF_Cfg.Speed == 0)
            nrf.SetSpeed(nrf.SPEED_250Kbps);
        else if(NRF_Cfg.Speed == 1)
            nrf.SetSpeed(nrf.SPEED_1Mbps);
        else if(NRF_Cfg.Speed == 2)
            nrf.SetSpeed(nrf.SPEED_2Mbps);
        break;

    case ItemNum::SaveExit:
        EEPROM_Handle(EEPROM_Chs::SaveData);
        break;

    case ItemNum::Bluetooth:
        hc05.Power(!State_Bluetooth);
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

static float CursorPosNow = ItemStartY;
static float CursorPosLast = ItemStartY + menu.ItemNumDisplay_Max * 10;

static void Task_DrawCursorUpdate()
{
    CursorPosNow = ItemStartY + ItemDispalyPos * TextHeight + 1;
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
    XFS_Speak("载入设置");
    ClearPage();
    CursorPosNow = ItemStartY;
    CursorPosLast = screen.height();
    if(page.LastPage == PAGE_MainMenu)
    {
        ItemDispalyPos = menu.ItemSelect = menu.ItemTop = 0;
        menu.ItemXUpdating = menu.ItemSelectUpdating = menu.ItemTopUpdating = false;
    }
    
    menu.ItemTopUpdating = true;

    menu.CallbackFunc_ItemSelectEvent = When_ItemSelect;
    menu.UpdateItem(ItemNum::Mode,    ".Model:"   + StrModel[CTRL.Info.CtrlObject]);
    menu.UpdateItem(ItemNum::Btc,     ".Btc:"     + StrBtc[Bluetooth_ConnectObject]);
    menu.UpdateItem(ItemNum::Addr,    ".Address:" + String(NRF_Cfg.Address));
    menu.UpdateItem(ItemNum::Freq,    ".Freq:"    + String(2400 + nrf.GetFreqency()) + "MHz");
    menu.UpdateItem(ItemNum::Speed,   ".Speed:"   + StrSpeed[nrf.GetSpeed()]);
    menu.UpdateItem(ItemNum::PassBack,   ".PassBack",  menu.TYPE_Bool, (int)&State_PassBack);
    menu.UpdateItem(ItemNum::Handshake,  ".Handshake", menu.TYPE_Bool, (int)&State_Handshake);
    menu.UpdateItem(ItemNum::Sound,      ".Sound",     menu.TYPE_Bool, (int)&State_BuzzSound);
    menu.UpdateItem(ItemNum::Bluetooth,  ".Bluetooth", menu.TYPE_Bool, (int)&State_Bluetooth);
    menu.UpdateItem(ItemNum::PowerSaving, ".LowPower",  menu.TYPE_Bool, (int)&State_PowerSaving);
    menu.UpdateItem(ItemNum::SetJoystick,  ".Joystick   >",   menu.TYPE_PageJump, PAGE_SetJoystick);
    menu.UpdateItem(ItemNum::SetBlueTooth, ".BlueTooth  >",   menu.TYPE_PageJump, PAGE_SetBluetooth);
    menu.UpdateItem(ItemNum::SetGravity,   ".Gravity    >",   menu.TYPE_PageJump, PAGE_SetGravity);
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
    if(page.NextPage == PAGE_MainMenu)
        XFS_Speak("退出设置");

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

    if(btOK || btEcd)
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

static void EncoderEvent()
{
    if(ecd > 0)
        menu.SelectMove(+1);
    if(ecd < 0)
        menu.SelectMove(-1);
}

void PageRegister_Options(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
