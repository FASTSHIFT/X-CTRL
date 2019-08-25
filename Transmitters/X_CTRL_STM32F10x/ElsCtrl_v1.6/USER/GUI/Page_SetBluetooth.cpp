#include "FileGroup.h"
#include "GUI_Private.h"

bool State_Bluetooth = true;
uint8_t Bluetooth_ConnectObject = BC_Type::BC_XFS;

Bluetooth_HC05 hc05(&Serial);
static const uint32_t UseBaudRate[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200};
#define UseBaudRate_Size __Sizeof(UseBaudRate)
static uint8_t BaudRateSelect = 0;
static String Password;
static bool Role;

static String ItemStr[4];
static int16_t ItemSelect = 0;
static bool ItemSelectUpdating = false;
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14
#define ItemSelect_MAX 4

static void UpdateItemStr()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        if(i == ItemSelect)
            screen.setTextColor(screen.Yellow, screen.Black);
        else
            screen.setTextColor(screen.White, screen.Black);
        
        screen.setCursor(ItemStartX, ItemStartY + i * 10);
        screen.print(ItemStr[i]);
    }
}

static void Setup()
{
    hc05.Init(115200);
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(20, StatusBar_POS + 10);
    screen.print("Enter AT Mode...");
    screen.setCursor(20, StatusBar_POS + 20);
    if(hc05.AT_Enter())
    {
        ItemSelect = 0;
        BaudRateSelect = 0;
        ItemSelectUpdating = false;
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("SUCCESS");
        ItemStr[0] = "1.Name:" + hc05.GetName();
        ItemStr[1] = "2.BaudRate:" + String(hc05.GetBaudRate());
        ItemStr[2] = "3.Password:" + hc05.GetPassword();
        ItemStr[3] = "4.Role:" + String(hc05.GetRole() ? "Master" : "Slaver");
        Role = hc05.GetRole();
        ClearPage();
        UpdateItemStr();
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("FAILED");
        PageDelay(1000);
        page.PageChangeTo(PAGE_Options);
    }
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
    ClearPage();
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(20, StatusBar_POS + 10);
    screen.print("Exit AT Mode...");
    hc05.SetBaudRate(UseBaudRate[BaudRateSelect]);
    hc05.SetPassword(Password);
    hc05.SetRole(Role);
    hc05.AT_Exit();
    Serial.begin(UseBaudRate[BaudRateSelect]);
}

static void ButtonPressEvent()
{
    if(btOK || btEcd)
    {
        switch(ItemSelect)
        {
        case 1:
            BaudRateSelect = (BaudRateSelect + 1) % UseBaudRate_Size;
            ItemStr[1] = "2.BaudRate:" + String(UseBaudRate[BaudRateSelect]) + "  ";
            ItemSelectUpdating = true;
            break;
        case 2:
            static uint8_t i = 0;
            i++;
            i %= 10;
            Password = String(i * 1000 + i * 100 + i * 10 + i);
            ItemStr[2] = "3.Password:" + Password;
            ItemSelectUpdating = true;
            break;
        case 3:
            Role = ! Role;
            ItemStr[3] = "4.Role:" + String(Role ? "Master" : "Slaver");
            ItemSelectUpdating = true;
            break;
        }
    }
    if(btBACK)
    {
        page.PageChangeTo(PAGE_Options);
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

static void ButtonLongPressEvent()
{
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

void PageRegister_SetBluetooth()
{
#define ThisPage PAGE_SetBluetooth
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
    page.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
    page.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
