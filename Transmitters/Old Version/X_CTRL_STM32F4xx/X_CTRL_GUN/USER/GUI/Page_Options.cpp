#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_Options(2);

/*目录坐标*/
#define ItemStartY (StatusBar_Height+4)
#define ItemStartX (TEXT_WIDTH_1+8)
#define ItemEndX   (screen.width()-35)

/*实例化光标控件对象*/
static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, screen.height(), 8, 8);

/*实例化开关控件对象*/
//static LightGUI::BoolSwitch<SCREEN_CLASS> Switch(&screen, screen.width() - 35, ItemStartY + 1 * 10, 15, 9, 2);

/*自定义选项字符串*/
const char* StrBtc[BC_Type::BC_END] = {"XFS", "PHE"};
static const char* StrSpeed[] = {"250Kbps", "1Mbps", "2Mbps"};

/*当前选项显示的位置*/
static int16_t ItemDispalyPos;

/*选项闪烁标志位*/
static bool ItemFlashUpdating = false;

/*申请退出闪烁状态标志位*/
static bool ItemFlashUpdating_ReqExit = false;

/*暂停闪烁标志位*/
static bool ItemFlashUpdating_Pause = false;

/*选项编号命名空间*/
namespace ItemNum
{
/*选项枚举*/
enum
{
    /*Value*/
    Btc,
    Addr,
    Freq,
    Speed,
    
    /*Switch*/
    PassBack,
    Handshake,
    FHSS,
    Sound,
    Vibration,
    Bluetooth,
    CPU_Usage,
    IdleWarn,
    SignWarn,
    
    /*Config*/
    SetJoystick,
    SetChannel,
    SetGravity,
    SetBlueTooth,
    
    /*Tools*/
    FreqGraph,
    Jamming,
    FileExplorer,
    
    /*Others*/
    About,
    SaveExit,
    MAX
};
}

/*实例化菜单管理器对象*/
/*
 *ItemNum::MAX -> 选项最大个数
 *           7 -> 单页显示选项数量
 *       &page -> 页面调度器地址
 */
static MenuManager menu(
    ItemNum::MAX,
    (screen.height() - StatusBar_Height) / TEXT_HEIGHT_2 - 1,
    &page
);

/*实例化滚动条控件对象*/
static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(
    &screen,
    screen.width() + 5,
    StatusBar_Height + 4,
    4,
    menu.ItemNumDisplay_Max * TEXT_HEIGHT_2,
    1,
    menu.ItemNumDisplay_Max * TEXT_HEIGHT_2 * (menu.ItemNumDisplay_Max / (float)menu.ItemNum_Max)
);

/**
  * @brief  清除指定的选项区域
  * @param  pos:指定的选项区域
  * @retval 无
  */
static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(
        ItemStartX,
        ItemStartY + pos * TEXT_HEIGHT_2,
        Scroll.X - ItemStartX,
        TEXT_HEIGHT_2,
        screen.Black
    );
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
    DrawBoolSwitch(x, ItemStartY + pos * TEXT_HEIGHT_2 + 3, 15, 9, 2, sw);
}

/**
  * @brief  更新光标位置
  * @param  无
  * @retval 无
  */
static void UpadateCursor()
{
    ItemDispalyPos = menu.GetItemDispalyPos();
}

/**
  * @brief  更新选项顶部
  * @param  无
  * @retval 无
  */
static void UpdateItemTop()
{
    menu.UpdateItemStrBuffer();
    Scroll.setScroll(1.0f - ((float)menu.ItemTop / (float)(menu.ItemNum_Max - menu.ItemNumDisplay_Max)));
    screen.setTextColor(screen.White, screen.Black);
    for(int16_t i = 0; i < menu.ItemNumDisplay_Max; i++)
    {
        ClearItemPos(i);
        screen.setCursor(ItemStartX, ItemStartY + i * TEXT_HEIGHT_2);
        screen.printfX(menu.ItemStrBuffer[i]);

        int16_t nowItemNum = (menu.ItemTop + i) % menu.ItemNum_Max;
        if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
        {
            DrawItemSwitch(i, *((bool*)menu.ItemList[nowItemNum].Data));
        }
        else if(menu.ItemList[nowItemNum].Type == menu.TYPE_PageJump)
        {
            screen.setCursor(ItemEndX + 6, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(">");
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
    screen.setCursor(ItemStartX, ItemStartY + pos * TEXT_HEIGHT_2);
    screen.printfX(menu.ItemStrBuffer[pos]);

    int16_t nowItemNum = (menu.ItemTop + pos) % menu.ItemNum_Max;
    if(menu.ItemList[nowItemNum].Type == menu.TYPE_Bool)
        DrawItemSwitch(pos, *((bool*)menu.ItemList[nowItemNum].Data));
}

/**
  * @brief  闪烁指定行选项
  * @param  pos:行
  * @retval 无
  */
static void FlashItemPos(uint8_t pos, uint32_t time = 500)
{
    static bool isDisplay = true;
    __IntervalExecute(
        (
            (isDisplay ? UpdateItemPos(pos) : ClearItemPos(pos)),
            isDisplay = !isDisplay
        ),
        time
    );

    if(ItemFlashUpdating_Pause)
    {
        isDisplay = true;
    }

    if(ItemFlashUpdating_ReqExit)
    {
        UpdateItemPos(pos);
        isDisplay = true;
        ItemFlashUpdating = ItemFlashUpdating_ReqExit = false;
    }
}

/**
  * @brief  选项被增减事件
  * @param  无
  * @retval 无
  */
static void When_ItemSelect_IncDec(int8_t step)
{
    String str_temp;
    switch(menu.ItemSelect)
    {
    case ItemNum::Btc:
        __ValueStep(CTRL.Bluetooth.ConnectObject, step, BC_Type::BC_END);
        menu.UpdateItem(ItemNum::Btc, ".Btc:" + String(StrBtc[CTRL.Bluetooth.ConnectObject]));
        break;

    case ItemNum::Addr:
        __ValueStep(NRF_Cfg.Address, step, (sizeof(NRF_AddressConfig) / 5));
        str_temp.sprintf(".Address:%d", NRF_Cfg.Address);
        menu.UpdateItem(ItemNum::Addr, str_temp);
        nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);
        break;

    case ItemNum::Freq:
        __ValueStep(NRF_Cfg.Freq, step, 126);
        str_temp.sprintf(".Freq:%dMHz", 2400 + NRF_Cfg.Freq);
        menu.UpdateItem(ItemNum::Freq, str_temp);
        nrf.SetFreqency(NRF_Cfg.Freq);
        break;

    case ItemNum::Speed:
        __ValueStep(NRF_Cfg.Speed, step, 3);
        menu.UpdateItem(ItemNum::Speed, ".Speed:" + String(StrSpeed[NRF_Cfg.Speed]));
        nrf.SetSpeed(NRF_Cfg.Speed);
        break;
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
    case ItemNum::Btc:
        ItemFlashUpdating = true;
        break;

    case ItemNum::Addr:
        ItemFlashUpdating = true;
        break;

    case ItemNum::Freq:
        /*在握手使能时，拦截事件，禁止更改*/
        if(CTRL.State.Handshake)
        {
            BuzzTone(100, 20);
            break;
        }
        ItemFlashUpdating = true;
        break;

    case ItemNum::Speed:
        ItemFlashUpdating = true;
        break;

    case ItemNum::SaveExit:
        if(EEPROM_Handle(EEPROM_Chs::SaveData))
        {
            BuzzTone(2000, 50);
        }
        else
        {
            BuzzTone(50, 50);
        }
        page.PagePop();
        break;

    case ItemNum::Bluetooth:
        hc05.Power(!CTRL.Bluetooth.Enable);
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
    __SemaphoreTake(menu.ItemSelectUpdating, UpadateCursor());
    __SemaphoreTake(menu.ItemTopUpdating,    UpdateItemTop());
    __SemaphoreTake(menu.ItemXUpdating,      UpdateItemPos(ItemDispalyPos));

    if(ItemFlashUpdating)
    {
        FlashItemPos(ItemDispalyPos, 250);
    }
}

/*当前光标位置*/
static float CursorPosNow = ItemStartY;

/*上一次的光标位置*/
static float CursorPosLast = ItemStartY + menu.ItemNumDisplay_Max * 10;

/**
  * @brief  光标位置更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawCursorUpdate()
{
    CursorPosNow = ItemStartY + ItemDispalyPos * TEXT_HEIGHT_2 + 5;

    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4f;
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



/*********************************** Events ************************************/

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
    CursorPosLast = screen.height();
    Scroll.Color_FM = Scroll.Color_SB = screen.White;
    if(page.LastPage == PAGE_MainMenu)
    {
        ItemDispalyPos = menu.ItemSelect = menu.ItemTop = 0;
        menu.ItemXUpdating = menu.ItemSelectUpdating = false;
    }

    menu.ItemTopUpdating = true;

    /*菜单选项选中事件绑定*/
    menu.CallbackFunc_ItemSelectEvent = When_ItemSelect;
    /*菜单选项注册*/
    /*Value*/
    menu.UpdateItem(ItemNum::Btc,     ".Btc:"     + String(StrBtc[CTRL.Bluetooth.ConnectObject]));
    menu.UpdateItem(ItemNum::Addr,    ".Address:" + String(NRF_Cfg.Address));
    menu.UpdateItem(ItemNum::Freq,    ".Freq:"    + String(2400 + nrf.GetFreqency()) + "MHz");
    menu.UpdateItem(ItemNum::Speed,   ".Baud:"    + String(StrSpeed[nrf.GetSpeed()]));
    
    /*Switch*/
    menu.UpdateItem(ItemNum::PassBack,   ".PassBack",   menu.TYPE_Bool, (int)&CTRL.State.PassBack);
    menu.UpdateItem(ItemNum::Handshake,  ".Handshake",  menu.TYPE_Bool, (int)&CTRL.State.Handshake);
    menu.UpdateItem(ItemNum::FHSS,       ".FHSS",       menu.TYPE_Bool, (int)&CTRL.State.FHSS);
    menu.UpdateItem(ItemNum::Sound,      ".Sound",      menu.TYPE_Bool, (int)&CTRL.State.Sound);
    menu.UpdateItem(ItemNum::Vibration,  ".Vibration",  menu.TYPE_Bool, (int)&CTRL.State.Vibrate);
    menu.UpdateItem(ItemNum::Bluetooth,  ".Bluetooth",  menu.TYPE_Bool, (int)&CTRL.Bluetooth.Enable);
    menu.UpdateItem(ItemNum::CPU_Usage,  ".CPU Load",   menu.TYPE_Bool, (int)&CTRL.CPU.Enable);
    menu.UpdateItem(ItemNum::IdleWarn,   ".IdleWarn",   menu.TYPE_Bool, (int)&CTRL.State.IdleWarn);
    menu.UpdateItem(ItemNum::SignWarn,   ".SignWarn",   menu.TYPE_Bool, (int)&CTRL.State.SignWarn);
    
    /*Config*/
    menu.UpdateItem(ItemNum::SetJoystick,  ".Joystick",  menu.TYPE_PageJump, PAGE_SetJoystick);
    menu.UpdateItem(ItemNum::SetChannel,   ".Channel",   menu.TYPE_PageJump, PAGE_ChannelCfg);
    menu.UpdateItem(ItemNum::SetGravity,   ".Gravity",   menu.TYPE_PageJump, PAGE_SetGravity);
    menu.UpdateItem(ItemNum::SetBlueTooth, ".BlueTooth", menu.TYPE_PageJump, PAGE_SetBluetooth);
    
    /*Tools*/
    menu.UpdateItem(ItemNum::FreqGraph,   ".Spectrum",  menu.TYPE_PageJump, PAGE_FreqGraph);
    menu.UpdateItem(ItemNum::Jamming,     ".Jamming",   menu.TYPE_PageJump, PAGE_Jamming);
    menu.UpdateItem(ItemNum::FileExplorer, ".Explorer", menu.TYPE_PageJump, PAGE_FileExplorer);
    
    /*Others*/
    menu.UpdateItem(ItemNum::About,        ".About",       menu.TYPE_PageJump, PAGE_About);
    menu.UpdateItem(ItemNum::SaveExit,     ".Save & Exit", menu.TYPE_None, 0);

    menu.UpdateItemStrBuffer();
    for(uint8_t i = 0; i < menu.ItemNumDisplay_Max; i++)
    {
        for(int16_t j = -20; j < ItemStartX - 2; j++)
        {
            screen.setOpacityX(__Map(j, -20, ItemStartX - 3, 0, 255));
            screen.setTextColor(screen.White, screen.Black);
            screen.setCursor(j, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(" " + menu.ItemStrBuffer[i]);
            screen.setOpacityX(255);
            PageDelay(1);
        }
        BuzzTone(1000 + 100 * i, 5);
        PageDelay(10);
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
    for(float i = 1.0f; i > 0.0f; i -= ((i * 0.01f) + 0.001f))
    {
        Scroll.Color_FM = Scroll.Color_SB = StrPrinter_ColorMap(screen.Black, screen.White, (uint8_t)(i * 15));
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        PageDelay(2);
    }
    ClearPage();
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
    {
        if(param == &btUP)
        {
            if(ItemFlashUpdating)
            {
                ItemFlashUpdating_Pause = true;
                When_ItemSelect_IncDec(+1);
            }
            else
                menu.SelectMove(+1);//菜单光标下移
        }
        if(param == &btDOWN)
        {
            if(ItemFlashUpdating)
            {
                ItemFlashUpdating_Pause = true;
                When_ItemSelect_IncDec(-1);
            }
            else
                menu.SelectMove(-1);//菜单光标上移
        }
    }
    if(event == EVENT_ButtonPress)
    {
        if(param == &btOK || param == &btEcd)
        {
            if(ItemFlashUpdating)
                ItemFlashUpdating_ReqExit = true;
            else
                menu.ItemSelectEvent();//触发选项被选中事件
        }

        if(param == &btBACK)
        {
            if(ItemFlashUpdating)
                ItemFlashUpdating_ReqExit = true;
            else
                page.PagePop();//页面跳转至主菜单
        }
    }
    if(event == EVENT_ButtonRelease)
    {
        ItemFlashUpdating_Pause = false;
    }
}

/**
  * @brief  设置页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Options(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
