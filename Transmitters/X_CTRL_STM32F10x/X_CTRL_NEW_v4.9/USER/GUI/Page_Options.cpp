#include "FileGroup.h"
#include "GUI_Private.h"
#include "MenuManager.h"
#include "ComPrivate.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_Options(2);

/*目录起始坐标*/
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14
#define ItemEndX   (screen.width()-35)

/*实例化滚动条控件对象*/
static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(&screen, screen.width() + 5, StatusBar_POS + 6, 4, 43, 1, 10);

/*实例化光标控件对象*/
static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, ItemStartY, 8, 8);

/*实例化开关控件对象*/
//static LightGUI::BoolSwitch<SCREEN_CLASS> Switch(&screen, screen.width() - 35, ItemStartY + 1 * 10, 15, 9, 2);

/*自定义选项字符串*/
static String StrModel[] = {"COMMON", "CAR-DS", "CAR-SS"};
static String StrBtc[] = {"HMI", "XFS"};
static String StrSpeed[] = {"250Kbps", "1Mbps", "2Mbps"};

/*当前选项显示的位置*/
static int16_t ItemDispalyPos;

/*选项编号命名空间*/
namespace ItemNum {
/*选项枚举*/
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
    CPU_Usage,
    NoOperationMonitor,
    SetJoystick,
    SetBlueTooth,
    SetGravity,
    FreqGraph,
    About,
    SaveExit,
    MAX
};
}

/*实例化菜单管理器对象*/
/*
 *ItemNum::MAX -> 选项最大个数
 *           4 -> 单页显示选项数量
 *       &page -> 页面调度器地址
 */
static MenuManager menu(ItemNum::MAX, 4, &page);

/**
  * @brief  清除指定的选项区域
  * @param  pos:指定的选项区域
  * @retval 无
  */
static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(ItemStartX, ItemStartY + pos * 10, ItemStartX + 94, 10, screen.Black);
}

/**
  * @brief  在指定坐标绘制布尔开关控件
  * @param  x:X起始坐标
  * @param  y:Y起始坐标
  * @param  w:控件宽度
  * @param  h:控件高度
  * @param  r:内圆半径
  * @param  sw:开关状态
  * @retval 无
  */
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

/**
  * @brief  在指定行绘制布尔开关
  * @param  pos:行
  * @param  sw:开关状态
  * @param  x:X坐标
  * @retval 无
  */
static void DrawItemSwitch(uint8_t pos, bool sw, int16_t x = ItemEndX)
{
    DrawBoolSwitch(x, ItemStartY + pos * 10, 15, 9, 2, sw);
}

/**
  * @brief  更新光标位置
  * @param  无
  * @retval 无
  */
static void UpadateCursor()
{
    ItemDispalyPos = menu.ItemSelect - menu.ItemTop;
}

/**
  * @brief  更新选项顶部
  * @param  无
  * @retval 无
  */
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
        {
            DrawItemSwitch(i, *((bool*)menu.ItemList[nowItemNum].Data));
        }
        else if(menu.ItemList[nowItemNum].Type == menu.TYPE_PageJump)
        {
            screen.setCursor(ItemEndX + 10, ItemStartY + i * (TEXT_HEIGHT_1 + 2));
            screen.print('>');
        }
    }
}

/**
  * @brief  更新指定行选项
  * @param  pos:行
  * @retval 无
  */
static void UpdateItemPos(uint8_t pos)
{
    menu.UpdateItemStrBuffer();
    screen.setTextColor(screen.White, screen.Black);
    ClearItemPos(pos);
    screen.setCursor(ItemStartX, ItemStartY + pos * 10);
    screen.print(menu.ItemStrBuffer[pos]);

    int16_t nowItemNum = (menu.ItemTop + pos) % menu.ItemNum_Max;
    if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
    {
        DrawItemSwitch(pos, *((bool*)menu.ItemList[nowItemNum].Data));
    }
}

/**
  * @brief  选项被选中事件
  * @param  无
  * @retval 无
  */
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
        nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);
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

/**
  * @brief  菜单更新任务
  * @param  无
  * @retval 无
  */
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

/*光标目标位置*/
static float CursorTarget[4] = {ItemStartY, ItemStartY + 10, ItemStartY + 20, ItemStartY + 30};

/*当前光标位置*/
static float CursorPosNow = ItemStartY;

/*上一次的光标位置*/
static float CursorPosLast = ItemStartY + 30;

/**
  * @brief  光标位置更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawCursorUpdate()
{
    CursorPosNow = CursorTarget[ItemDispalyPos];
    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4;
    }

    ItemCursor.setPosition(ItemStartX - 8, CursorPosLast);
}

/**
  * @brief  自定义光标回调函数
  * @param  无
  * @retval 无
  */
static void DrawCursor_Callback()
{
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setCursor(ItemCursor.X, ItemCursor.Y);
    screen.print('>');
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    XFS_Speak("载入设置");
    ClearPage();
    CursorPosNow = ItemStartY;
    CursorPosLast = ItemStartY + 30;
    if(page.LastPage == PAGE_MainMenu)
    {
        ItemDispalyPos = menu.ItemSelect = menu.ItemTop = 0;
        menu.ItemXUpdating = menu.ItemSelectUpdating = menu.ItemTopUpdating = false;
    }
    else
        menu.ItemTopUpdating = true;

    /*菜单选项注册*/
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
    menu.UpdateItem(ItemNum::CPU_Usage,  ".CPU Load",   menu.TYPE_Bool, (int)&State_DisplayCPU_Usage);
    menu.UpdateItem(ItemNum::NoOperationMonitor, ".OpaMonit",menu.TYPE_Bool, (int)&State_NoOperationMonitor);
    menu.UpdateItem(ItemNum::SetJoystick,  ".Joystick", menu.TYPE_PageJump, PAGE_SetJoystick);
    menu.UpdateItem(ItemNum::SetBlueTooth, ".BlueTooth",menu.TYPE_PageJump, PAGE_SetBluetooth);
    menu.UpdateItem(ItemNum::SetGravity,   ".Gravity",  menu.TYPE_PageJump, PAGE_SetGravity);
    menu.UpdateItem(ItemNum::FreqGraph,    ".Spectrum", menu.TYPE_PageJump, PAGE_FreqGraph);
    menu.UpdateItem(ItemNum::About,        ".About",    menu.TYPE_PageJump, PAGE_About);
    menu.UpdateItem(ItemNum::SaveExit,     ".Save & Exit", menu.TYPE_PageJump, PAGE_MainMenu);

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

    /*注册自定义光标回调函数*/
    ItemCursor.DisplayCallbackFunction = DrawCursor_Callback;

    /*注册任务*/
    mtm_Options.TaskRegister(0, Task_MenuUpdate, 50);
    mtm_Options.TaskRegister(1, Task_DrawCursorUpdate, 20);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    mtm_Options.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    if(page.NextPage == PAGE_MainMenu)
        XFS_Speak("退出设置");

    /*滚动条向左滑动动画*/
    for(float i = 1.0; i > 0.0; i -= ((i * 0.01) + 0.001))
    {
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        PageDelay(1);
    }
    ClearPage();
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    if(btDOWN)
        menu.SelectMove(+1);//菜单光标下移
    if(btUP)
        menu.SelectMove(-1);//菜单光标上移

    if(btOK || btEcd)
        menu.ItemSelectEvent();//触发选项被选中事件

    if(btBACK)
        page.PageChangeTo(PAGE_MainMenu);//页面跳转至主菜单
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
    if(btDOWN)
        menu.SelectMove(+1);//菜单光标下移
    if(btUP)
        menu.SelectMove(-1);//菜单光标上移
}

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
static void EncoderEvent()
{
    if(ecd > 0)
        menu.SelectMove(+1);//顺时针旋转，菜单光标下移
    if(ecd < 0)
        menu.SelectMove(-1);//逆时针旋转，菜单光标上移
}

/**
  * @brief  设置页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Options(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
