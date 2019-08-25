#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

static int16_t ItemSelect = 0;
static bool ItemSelectUpdating = true;
#define ItemStartY 2
#define ItemStartX (StatusBarPos+4)
static uint8_t ItemSelect_MAX = 0;
static bool ExitHandshake = false;
static bool HaveButtonEvent = false;

static void UpdateItemStr()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        if(!HandshakePack_Slave[i].HeadCode)
            continue;

        if(i == ItemSelect)
        {
            screen.setTextColor(screen.Black, screen.White);
        }
        else
        {
            screen.setTextColor(screen.White, screen.Black);
        }
        screen.setCursor(ItemStartX, ItemStartY + i * 10);
        screen.printf("ID:0x%x", HandshakePack_Slave[i].ID);
        screen.printf(" %s", HandshakePack_Slave[i].Description);
    }
}

static void Setup()
{   
    ClearPage();
    State_RF = OFF;
    ExitHandshake = false;
    ItemSelectUpdating = true;
    HaveButtonEvent = false;
    ItemSelect_MAX = 0;
    ItemSelect = 0;
    HandshakeRun(HandshakeState_Prepare);

    LightGUI::ScrollBar<SCREEN_CLASS> SearchScroll(&screen, ItemStartX, screen.height(), screen.width() / 2, 6, 0, 4);

    screen.setCursor(ItemStartX, ItemStartY);
    screen.setTextColor(screen.White, screen.Black);
    screen.print("Searching...");

    for(uint8_t y = screen.height(); y > screen.height() - 10; y--)
    {
        SearchScroll.setPosition(SearchScroll.X, y);
        PageDelay(10);
    }

    uint32_t time = millis();
    uint8_t ItemSelect_Last = 0;
    while(millis() - time < 5000)
    {
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);
        if(ItemSelect_MAX != ItemSelect_Last)
        {
            screen.setCursor(ItemStartX, ItemStartY + 10);
            screen.setTextColor(screen.White, screen.Black);
            screen.printf("Find %d Slave...\r\n", ItemSelect_MAX);
            ItemSelect_Last = ItemSelect_MAX;
        }

        SearchScroll.setScroll((millis() % 400) / 400.0);
        PageDelay(1);

        if(HaveButtonEvent) break;
    }

    screen.setCursor(ItemStartX, ItemStartY + 20);
    if(ItemSelect_MAX)
    {
//        screen.setTextColor(screen.Green, screen.Black);
//        screen.print("Search Done!");
    }
    else
    {
//        screen.setTextColor(screen.Red, screen.Black);
//        screen.print("Not Found!");
        ExitHandshake = true;
    }

    for(uint8_t y = SearchScroll.Y; y <= screen.height(); y++)
    {
        SearchScroll.setPosition(SearchScroll.X, y);
        PageDelay(20);
    }

    ClearPage();
}

static void Loop()
{
    if(ExitHandshake)
        page.PageChangeTo(PAGE_CtrlInfo);
    if(ItemSelectUpdating)
    {
        UpdateItemStr();
        ItemSelectUpdating = false;
    }
}

static void Exit()
{
    ClearPage();

    if(ExitHandshake)
    {
#ifdef TIM_Handshake
        TIM_Cmd(TIM_Handshake, DISABLE);
#endif
        return;
    }

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(ItemStartX, ItemStartY);
    screen.print(HandshakePack_Slave[ItemSelect].Description);
    screen.setCursor(ItemStartX, ItemStartY + 10);
    screen.print("Connecting...");
    uint32_t timeout = millis();
    bool IsTimeout = false;
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        if(millis() - timeout > 10000)
        {
            screen.setTextColor(screen.Red, screen.Black);
            screen.setCursor(ItemStartX, ItemStartY + 10);
            screen.printf("Timeout");
            IsTimeout = true;
            break;
        }
        PageDelay(1);
    }
    HandshakeRun(HandshakeState_Connected);

    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;

    if(!IsTimeout)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.setCursor(ItemStartX, ItemStartY + 10);
        screen.printf("Success");
    }

    ClearPage();
}

static void ButtonPressEvent()
{
    if(btOK)
    {
        page.PageChangeTo(PAGE_CtrlInfo);
    }

    if(btDOWN)
    {
        ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
    if(btUP)
    {
        ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }

    HaveButtonEvent = true;
}

void PageRegister_Handshake(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
