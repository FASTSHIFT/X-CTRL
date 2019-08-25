#include "FileGroup.h"
#include "GUI_Private.h"

#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14

MillisTaskManager mtm_Options(2);

static Bitmap Bitmap_Option(oled, (uint8_t*)gImage_Bitmap_Option, 0, 0, 30, 30);
static ScrollBar Scroll(oled, oled.width() + 5, StatusBar_POS + 6, 4, 43, 1, 10);
static Cursor ItemCursor(oled, ItemStartX, ItemStartY, 8, 8);

String GetModel()
{
    if(CTRL.Info.CtrlObject == X_COMMON)return "COMMON";
    else if(CTRL.Info.CtrlObject == CAR_ServoSteering)return "CAR-SS";
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)return "CAR-DS";
    else return "UNKNOW";
}

String GetPattern()
{
    if(ConnectState.Pattern == Pattern_NRF)return "NRF24";
    else if(ConnectState.Pattern == Pattern_USART)return "HC-05";
    else return "UNKNOW";
}

static String GetSpeed()
{
    if(NRF_Cfg.Speed == 0) return "250Kbps";
    else if(NRF_Cfg.Speed == 1) return "1Mbps";
    else if(NRF_Cfg.Speed == 2) return "2Mbps";
    else return "UNKNOW";
}

#define clearItem() fillRect(ItemStartX,ItemStartY,ItemStartX+94,ItemStartY+40,BLACK)
#define clearItemx(item) fillRect(ItemStartX,ItemStartY+item*10,ItemStartX+94,10,BLACK)
#define updateItemx(item,text,data) (ItemStr[item]=String(item+1)+text+data)

enum ItemNum {
    ItemNum_Mode = 0,
    ItemNum_Pattern,
    ItemNum_Addr,
    ItemNum_Freq,
    ItemNum_Speed,
    ItemNum_PassBack,
    ItemNum_Sound,
    ItemNum_HMI,
    ItemNum_SetJoystick,
    ItemNum_SetBlueTooth,
    ItemNum_SetGravity,
    ItemNum_About,
    ItemNum_SaveExit,
    ItemNum_MAX
};
static String ItemStr[ItemNum_MAX + 1];
static String ItemStrBuffer[4];
static int16_t ItemSelect = 0;
static int16_t ItemTop = 0;
static int16_t ItemPoint = 0;
static bool ItemSelectUpdating;
static bool ItemTopUpdating;
static bool ItemXUpdating;

static void UpdateItemStrBuffer()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        ItemStrBuffer[i] = ItemStr[(ItemTop + i) % (ItemNum_MAX + 1)];
    }
}

static void UpadateCursor()
{
    ItemPoint = ItemSelect - ItemTop;
}

static void UpdateItemTop()
{
    Scroll.setScroll(1.0 - ((float)ItemTop / (float)(ItemNum_MAX - 4)));

    UpdateItemStrBuffer();
    oled.setTextColor(WHITE, BLACK);
    for(uint8_t i = 0; i < 4; i++)
    {
        oled.clearItemx(i);
        oled.setCursor(ItemStartX, ItemStartY + i * 10);
        oled.print(ItemStrBuffer[i]);
    }
}

static void UpdateItemX(uint8_t item)
{
    UpdateItemStrBuffer();
    oled.setTextColor(WHITE, BLACK);
    oled.clearItemx(item);
    oled.setCursor(ItemStartX, ItemStartY + item * 10);
    oled.print(ItemStrBuffer[item]);
}

static void Task_ItemScroll()
{
    if(ItemSelectUpdating)
    {
        UpadateCursor();
        ItemSelectUpdating = false;
    }

    if(ItemTopUpdating)
    {
        UpdateItemTop();
        ItemTopUpdating = false;
    }

    if(ItemXUpdating)
    {
        UpdateItemX(ItemPoint);
        ItemXUpdating = false;
    }
}

static const float CursorTarget[4] = {ItemStartY, ItemStartY + 10, ItemStartY + 20, ItemStartY + 32};
static float CursorPosNow = ItemStartY;
static float CursorPosLast = ItemStartY + 30;

static void Task_DrawCursorUpdate()
{
    CursorPosNow = CursorTarget[ItemPoint];
    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4;
    }

    ItemCursor.setPosition(ItemStartX - 8, CursorPosLast);
}

void DrawCursor_Handler()
{
    oled.setCursor(ItemCursor.X, ItemCursor.Y);
    oled.print('>');
}

static void Setup()
{
    oled.clearPage();
    CursorPosNow = ItemStartY;
    CursorPosLast = ItemStartY + 30;
    if(pm.LastPage == PAGE_MainMenu)
    {
        ItemPoint = ItemSelect = ItemTop = 0;
        ItemXUpdating = ItemSelectUpdating = ItemTopUpdating = false;
    }

    updateItemx(ItemNum_Mode, ".Model:", GetModel());
    updateItemx(ItemNum_Pattern, ".Pattern:", GetPattern());
    updateItemx(ItemNum_Addr, ".Address:", String(NRF_Cfg.Address));
    updateItemx(ItemNum_Freq, ".Freq:", String(2400+NRF_Cfg.Freq)+"MHz");
    updateItemx(ItemNum_Speed, ".Speed:", String(GetSpeed()));
    updateItemx(ItemNum_PassBack, ".PassBack:", String(PassBack_State ? "ON" : "OFF"));
    updateItemx(ItemNum_Sound, ".Sound:", String(BuzzSound_State ? "ON" : "OFF"));
    updateItemx(ItemNum_HMI, ".HMI:", String(HMI_State ? "ON" : "OFF"));
    updateItemx(ItemNum_SetJoystick, ".Set Joystick", "");
    updateItemx(ItemNum_SetBlueTooth, ".Set BlueTooth", "");
    updateItemx(ItemNum_SetGravity, ".Set Gravity", "");
    updateItemx(ItemNum_About, ".About", "");
    updateItemx(ItemNum_SaveExit, ".Save & Exit", "");

    oled.setTextColor(WHITE, BLACK);
    UpdateItemStrBuffer();
    for(uint8_t i = 0; i < 4; i++)
    {
        for(int16_t j = -20; j <= ItemStartX - 6; j += 4)
        {
            oled.setCursor(j, ItemStartY + i * 10);
            oled.print(' ');
            oled.print(ItemStrBuffer[i]);
        }
        BuzzTone(1000 + 100 * i, 5);
    }

    Scroll.setScroll(1.0);
    for(uint8_t i = oled.width() + 5; i > oled.width() - 7; i--)
    {
        Scroll.setPosition(i, Scroll.Y);
        delay(10);
    }

    ItemCursor.DisplayCallbackFunction = DrawCursor_Handler;

    mtm_Options.TaskRegister(0, Task_ItemScroll, 50);
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
        Scroll.setPosition(oled.width()*i, Scroll.Y);
		delayMicroseconds(200);
    }
    oled.clearPage();
}

void ItemSelectMove(char dir)
{
    if(dir == '+')
    {
        ItemSelect = (ItemSelect + 1) % ItemNum_MAX;
    }
    else if(dir == '-')
    {
        ItemSelect = (ItemSelect + ItemNum_MAX - 1) % ItemNum_MAX;
    }
    ItemSelectUpdating = true;
    if(ItemSelect - ItemTop > 3)
    {
        ItemTop = ItemSelect - 3;
        ItemTopUpdating = true;
    }
    if(ItemTop > ItemSelect)
    {
        ItemTop = ItemSelect;
        ItemTopUpdating = true;
    }
}

static void ButtonPressEvent()
{
    if(btDOWN)
    {
        ItemSelectMove('+');
    }
    if(btUP)
    {
        ItemSelectMove('-');
    }


    if(btOK)
    {
        switch(ItemSelect)
        {
        case ItemNum_Mode:
            CTRL.Info.CtrlObject = ( CTRL.Info.CtrlObject + 1) % 3;
            updateItemx(ItemNum_Mode, ".Model:", GetModel());
            ItemXUpdating = true;
            break;
        case ItemNum_Pattern:
            ConnectState.Pattern = ! ConnectState.Pattern;
            updateItemx(ItemNum_Pattern, ".Pattern:", GetPattern());
            ItemXUpdating = true;
            break;
        case ItemNum_Addr:
            NRF_Cfg.Address ++;
            if(NRF_Cfg.Address >= (sizeof(NRF_AddressConfig) / 5))
                NRF_Cfg.Address = 0;
            updateItemx(ItemNum_Addr, ".Address:", String(NRF_Cfg.Address));
            nrf.SetAddress(NRF_AddressConfig + NRF_Cfg.Address * 5);
            ItemXUpdating = true;
            break;
        case ItemNum_Freq:
            NRF_Cfg.Freq +=10;
            if(NRF_Cfg.Freq > 120)NRF_Cfg.Freq = 0;
            updateItemx(ItemNum_Freq, ".Freq:", String(2400+NRF_Cfg.Freq)+"MHz");
            nrf.SetFreqency(NRF_Cfg.Freq);
            ItemXUpdating = true;
            break;
        case ItemNum_Speed:
            NRF_Cfg.Speed ++;
            NRF_Cfg.Speed %= 3;
            updateItemx(ItemNum_Speed, ".Speed:", GetSpeed());
            if(NRF_Cfg.Speed == 0)
                nrf.SetSpeed(nrf.SPEED_250Kbps);
            else if(NRF_Cfg.Speed == 1)
                nrf.SetSpeed(nrf.SPEED_1Mbps);
            else if(NRF_Cfg.Speed == 2)
                nrf.SetSpeed(nrf.SPEED_2Mbps);
            ItemXUpdating = true;
            break;
        case ItemNum_PassBack:
            PassBack_State = !PassBack_State;
            updateItemx(ItemNum_PassBack, ".PassBack:", String(PassBack_State ? "ON" : "OFF"));
            ItemXUpdating = true;
            break;
        case ItemNum_Sound:
            BuzzSound_State = !BuzzSound_State;
            updateItemx(ItemNum_Sound, ".Sound:", String(BuzzSound_State ? "ON" : "OFF"));
            ItemXUpdating = true;
            break;
        case ItemNum_HMI:
            HMI_State = !HMI_State;
            updateItemx(ItemNum_HMI, ".HMI:", String(HMI_State ? "ON" : "OFF"));
            ItemXUpdating = true;
            break;
        case ItemNum_SetJoystick:
            pm.PageChangeTo(PAGE_SetJoystick);
            break;
        case ItemNum_SetBlueTooth:
            pm.PageChangeTo(PAGE_SetBluetooth);
            break;
        case ItemNum_SetGravity:
            pm.PageChangeTo(PAGE_SetGravity);
            break;
        case ItemNum_About:
            pm.PageChangeTo(PAGE_About);
            break;
        case ItemNum_SaveExit:
            EEPROM_Handle(SaveData);
            pm.PageChangeTo(PAGE_MainMenu);
            break;
        default:
            BuzzTone(100, 20);
            break;
        }
    }

    if(btBACK)
    {
        pm.PageChangeTo(PAGE_MainMenu);
    }
}

static void ButtonLongPressEvent()
{
    if(btDOWN)
    {
        if(!ItemSelectUpdating && !ItemTopUpdating)ItemSelectMove('+');
    }
    if(btUP)
    {
        if(!ItemSelectUpdating && !ItemTopUpdating)ItemSelectMove('-');
    }
}

void PageRegister_Options()
{
#define ThisPage PAGE_Options
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
    pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
