#include "FileGroup.h"
#include "GUI_Private.h"

#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14
#define clearItem() fillRect(ItemStartX,ItemStartY,ItemStartX+94,ItemStartY+40,screen.Black)
#define clearItemx(item) fillRect(ItemStartX,ItemStartY+item*10,ItemStartX+94,10,screen.Black)

MillisTaskManager mtm_Options(2);

static Bitmap Bitmap_Option(screen, (uint16_t*)gImage_Bitmap_Option, 0, 0, 30, 30);
static ScrollBar Scroll(screen, screen.width() + 5, StatusBar_POS + 6, 4, 43, 1, 10);
static Cursor ItemCursor(screen, ItemStartX, ItemStartY, 8, 8);

static String StrModel[] = {"COMMON", "CAR-DS", "CAR-SS"};
static String StrPattern[] = {"NRF24", "HC-05"};
static String StrSpeed[] = {"250Kbps", "1Mbps", "2Mbps"};

typedef struct{
    String Str;
    uint8_t Type;
    uint8_t State;
}Item_TypeDef;

enum ItemType{
    TYPE_Text,
    TYPE_Switch,
    TYPE_PageJump
};

enum ItemNum {
    ItemNum_Mode = 0,
    ItemNum_Pattern,
    ItemNum_Addr,
    ItemNum_Freq,
    ItemNum_Speed,
    ItemNum_PassBack,
    ItemNum_Handshake,
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

static void updateItemx(uint8_t item, String text, String data)
{
    ItemStr[item] = String(item + 1) + text + data;
}

static void updateItemx(uint8_t item, String text, bool sw)
{
    ItemStr[item] = String(item + 1) + text;
}

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
    screen.setTextColor(screen.White, screen.Black);
    for(uint8_t i = 0; i < 4; i++)
    {
        screen.clearItemx(i);
        screen.setCursor(ItemStartX, ItemStartY + i * 10);
        screen.print(ItemStrBuffer[i]);
    }
}

static void UpdateItemX(uint8_t item)
{
    UpdateItemStrBuffer();
    screen.setTextColor(screen.White, screen.Black);
    screen.clearItemx(item);
    screen.setCursor(ItemStartX, ItemStartY + item * 10);
    screen.print(ItemStrBuffer[item]);
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

static void DrawCursor_Handler()
{
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setCursor(ItemCursor.X, ItemCursor.Y);
    screen.print('>');
}

static void Setup()
{
    screen.clearPage();
    CursorPosNow = ItemStartY;
    CursorPosLast = ItemStartY + 30;
    if(pm.LastPage == PAGE_MainMenu)
    {
        ItemPoint = ItemSelect = ItemTop = 0;
        ItemXUpdating = ItemSelectUpdating = ItemTopUpdating = false;
    }
    updateItemx(ItemNum_Mode, ".Model:", StrModel[CTRL.Info.CtrlObject]);
    updateItemx(ItemNum_Pattern, ".Pattern:", StrPattern[ConnectState.Pattern]);
    updateItemx(ItemNum_Addr, ".Address:", String(NRF_Cfg.Address));
    updateItemx(ItemNum_Freq, ".Freq:", String(2400+NRF_Cfg.Freq)+"MHz");
    updateItemx(ItemNum_Speed, ".Speed:", StrSpeed[NRF_Cfg.Speed]);
    updateItemx(ItemNum_PassBack, ".PassBack:", String(PassBack_State ? "ON" : "OFF"));
    updateItemx(ItemNum_Handshake, ".Handshake:", String(Handshake_State ? "ON" : "OFF"));
    updateItemx(ItemNum_Sound, ".Sound:", String(BuzzSound_State ? "ON" : "OFF"));
    updateItemx(ItemNum_HMI, ".HMI:", String(HMI_State ? "ON" : "OFF"));
    updateItemx(ItemNum_SetJoystick, ".Set Joystick", "");
    updateItemx(ItemNum_SetBlueTooth, ".Set BlueTooth", "");
    updateItemx(ItemNum_SetGravity, ".Set Gravity", "");
    updateItemx(ItemNum_About, ".About", "");
    updateItemx(ItemNum_SaveExit, ".Save & Exit", "");

    screen.setTextColor(screen.White, screen.Black);
    UpdateItemStrBuffer();
    for(uint8_t i = 0; i < 4; i++)
    {
        for(int16_t j = -20; j <= ItemStartX - 6; j += 4)
        {
            screen.setCursor(j, ItemStartY + i * 10);
            screen.print(' ');
            screen.print(ItemStrBuffer[i]);
        }
        BuzzTone(1000 + 100 * i, 5);
    }

    Scroll.setScroll(1.0);
    for(uint8_t i = screen.width() + 5; i > screen.width() - 7; i--)
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
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        delay(1);
    }
    screen.clearPage();
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


    if(btOK || btEcd)
    {
        switch(ItemSelect)
        {
        case ItemNum_Mode:
            CTRL.Info.CtrlObject = ( CTRL.Info.CtrlObject + 1) % 3;
            updateItemx(ItemNum_Mode, ".Model:", StrModel[CTRL.Info.CtrlObject]);
            ItemXUpdating = true;
            break;
        case ItemNum_Pattern:
            ConnectState.Pattern = ! ConnectState.Pattern;
            updateItemx(ItemNum_Pattern, ".Pattern:", StrPattern[ConnectState.Pattern]);
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
            updateItemx(ItemNum_Speed, ".Speed:", StrSpeed[NRF_Cfg.Speed]);
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
        case ItemNum_Handshake:
            Handshake_State = !Handshake_State;
            updateItemx(ItemNum_Handshake, ".Handshake:", String(Handshake_State ? "ON" : "OFF"));
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

static void EncoderEvent()
{
    if(ecd > 0)
    {
        if(!ItemSelectUpdating && !ItemTopUpdating)ItemSelectMove('+');
    }
    if(ecd < 0)
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
    pm.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
