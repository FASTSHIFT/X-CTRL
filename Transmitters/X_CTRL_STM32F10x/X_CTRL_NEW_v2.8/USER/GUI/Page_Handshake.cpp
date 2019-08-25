#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

static int16_t ItemSelect = 0;
static bool ItemSelectUpdating = true;
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14
static uint8_t ItemSelect_MAX = 0;
static bool ExitHandshake = false;

static void UpdateItemStr()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        if(!HandshakePack_Slave[i].HeadCode)continue;

        if(i == ItemSelect)
        {
            screen.setTextColor(screen.Yellow, screen.Black);
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
    screen.clearPage();
    RF_State = OFF;
    ExitHandshake = false;
    ItemSelectUpdating = true;
    ItemSelect_MAX = 0;
    ItemSelect = 0;
    HandshakeRun(HandshakeState_Prepare);
    
    screen.setCursor(ItemStartX,ItemStartY);
    screen.setTextColor(screen.White, screen.Black);
    screen.print("Searching...");
    uint32_t time = millis();
    while(millis() - time < 5000)
    {
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);
        if(ItemSelect_MAX)
        {
            screen.setCursor(ItemStartX,ItemStartY + 10);
            screen.printf("Find %d Slave...\r\n", ItemSelect_MAX);
        }
        if(btOK || btBACK || btEcd)
            break;
    }
    
    screen.setCursor(ItemStartX,ItemStartY + 20);
    if(ItemSelect_MAX)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("Search Done!");
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("Not Found!");
        
        pm.PageChangeTo(PAGE_MainMenu);
    }
    delay(500);
    screen.clearPage();
}

static void Loop()
{
    if(ItemSelectUpdating)
    {
        UpdateItemStr();
        ItemSelectUpdating = false;
    }
}

static void Exit()
{
    screen.clearPage();
    
    if(ExitHandshake)return;
    
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(ItemStartX,ItemStartY);
    screen.printf("Connect to %s...", HandshakePack_Slave[ItemSelect].Description);
    uint32_t timeout = millis();
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        if(millis() - timeout > 3000)
        {
            screen.setTextColor(screen.Red, screen.Black);
            screen.setCursor(ItemStartX,ItemStartY + 10);
            screen.printf("Failed!!");
            return;
        }
    }
    HandshakeRun(HandshakeState_Connected);
    
    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;
    PassBack_State = HandshakePack_Slave[ItemSelect].SupportPassback;
    
    screen.setTextColor(screen.Green, screen.Black);
    screen.setCursor(ItemStartX,ItemStartY + 10);
    screen.printf("Success!!");
    screen.clearPage();
}

static void ButtonPressEvent()
{
    if(btOK || btEcd)
    {
        pm.PageChangeTo(PAGE_CtrlInfo);
    }
    if(btBACK)
    {
        pm.PageChangeTo(PAGE_MainMenu);
        ExitHandshake = true;
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
}

static void EncoderEvent()
{
    if(ecd > 0)
    {
        ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
    if(ecd < 0)
    {
        ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
}

void PageRegister_Handshake()
{
    #define ThisPage PAGE_Handshake
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
//	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
	pm.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
