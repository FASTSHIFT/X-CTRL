#include "FileGroup.h"
#include "GUI_Private.h"

Bluetooth_HC05 hc05(Serial);
const uint32_t UseBaudRate[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200};
#define SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
static uint8_t BaudRateSelect = 0;
String Password;
static bool Role;
static bool IsATSuccess = false;

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
        {
            oled.setTextColor(BLACK, WHITE);
        }
        else
        {
            oled.setTextColor(WHITE, BLACK);
        }
        oled.setCursor(ItemStartX, ItemStartY + i * 10);
        oled.print(ItemStr[i]);
    }
}

static void Setup()
{
	IsATSuccess = false;
    oled.setCursor(20, StatusBar_POS + 10);
    oled.print("Enter AT Mode...");
    oled.setCursor(20, StatusBar_POS + 20);
    if(hc05.AT_Enter())
    {
        ItemSelect = 0;
        BaudRateSelect = 0;
        ItemSelectUpdating = false;
		IsATSuccess = true;
        oled.print("SUCCESS");
        ItemStr[0] = "1.Name:RC-CTRL" ;//+ hc05.GetName();
        ItemStr[1] = "2.BaudRate:" + String(hc05.GetBaudRate());
        ItemStr[2] = "3.Password:" + hc05.GetPassword();
        ItemStr[3] = "4.Role:" + String(hc05.GetRole() ? "Master" : "Slaver");
		Role = hc05.GetRole();
        oled.clearPage();
        UpdateItemStr();
    }
    else
    {
        oled.setTextColor(WHITE);
        oled.print("FAILED");
        delay(1000);
        pm.PageChangeTo(PAGE_Options);
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
    oled.clearPage();
	if(!IsATSuccess) 
	{
		Serial.begin(UseBaudRate[UART_BaudSelect_Default]);
		return;
	}
    oled.setCursor(20, StatusBar_POS + 10);
    oled.print("Exit AT Mode...");
    hc05.SetBaudRate(UseBaudRate[BaudRateSelect]);
	hc05.SetPassword(Password);
	hc05.SetRole(Role);
    hc05.AT_Exit();
	Serial.begin(UseBaudRate[BaudRateSelect]);
}

static void ButtonPressEvent()
{
    if(btOK)
    {
        switch(ItemSelect)
        {
        case 0:

            break;
        case 1:
            BaudRateSelect = (BaudRateSelect + 1) % SIZE(UseBaudRate);
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
			Role =! Role;
			ItemStr[3] = "4.Role:" + String(Role ? "Master" : "Slaver");
			ItemSelectUpdating = true;
            break;
        }
    }
    if(btBACK)
    {
        pm.PageChangeTo(PAGE_Options);
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

void PageRegister_SetBluetooth()
{
#define ThisPage PAGE_SetBluetooth
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
    pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
