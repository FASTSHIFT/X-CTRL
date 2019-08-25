#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

static int16_t ItemSelect = 0;
static bool ItemSelectUpdating = true;
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14
static uint8_t ItemSelect_MAX = 0;
static bool ExitHandshake = false;
static bool HaveButtonEvent = false;

static void UpdateItemStr()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        if(!HandshakePack_Slave[i].HeadCode)continue;

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

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    State_RF = OFF;//遥控关闭
    ExitHandshake = false;
    ItemSelectUpdating = true;
    HaveButtonEvent = false;
    ItemSelect_MAX = 0;
    ItemSelect = 0;
    
    /*主机准备握手*/
    HandshakeRun(HandshakeState_Prepare);

    /*实例化滚动条控件*/
    LightGUI::ProgressBar<SCREEN_CLASS> SearchProgress(&screen, 0, screen.height() - 6, screen.width(), 6, 0);
	SearchProgress.Color_FM = screen.Black;
	
    screen.setCursor(ItemStartX, ItemStartY);
    screen.setTextColor(screen.White, screen.Black);
    screen.print("Searching...");
    XFS_Speak("正在搜索");

    /*超时设置5000ms*/
    uint32_t time = millis();
    uint8_t ItemSelect_MAX_Last = 0;
    while(millis() - time < 5000)
    {
        /*获取从机列表数量*/
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);
        if(ItemSelect_MAX != ItemSelect_MAX_Last)
        {
            screen.setCursor(ItemStartX, ItemStartY + 10);
            screen.setTextColor(screen.White, screen.Black);
            screen.printf("Find %d Slave...", ItemSelect_MAX);
            ItemSelect_MAX_Last = ItemSelect_MAX;
        }

        /*滚动动画*/
//        SearchScroll.setScroll((millis() % 400) / 400.0);
		if(millis() / 400 % 2)
		{
			SearchProgress.Color_BG = screen.Black;
			SearchProgress.Color_PB = screen.White;
		}
		else
		{
			SearchProgress.Color_PB = screen.Black;
			SearchProgress.Color_BG = screen.White;
		}
        SearchProgress.setProgress((millis() % 400) / 400.0);
        PageDelay(1);

        /*是否强制跳出握手过程*/
        if(HaveButtonEvent) break;
    }

    /*显示搜索结果*/
    screen.setCursor(ItemStartX, ItemStartY + 20);
    if(ItemSelect_MAX > 0)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("Search Done!");
        XFS_Speak("搜索完成,找到 %d 个设备", ItemSelect_MAX);
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("Not Found!");
        XFS_Speak("未找到任何设备");
        ExitHandshake = true;
    }
    
    SearchProgress.Color_BG = screen.Black;
    SearchProgress.noDisplay();
	PageDelay(100);

    ClearPage();
}

static void Loop()
{
    if(ExitHandshake)
        page.PageChangeTo(PAGE_MainMenu);
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
    XFS_Speak("正在尝试连接到%s", HandshakePack_Slave[ItemSelect].Description);
    uint32_t timeout = millis();
    bool IsTimeout = false;
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        if(millis() - timeout > 2500)
        {
            screen.setTextColor(screen.Red, screen.Black);
            screen.setCursor(ItemStartX, ItemStartY + 10);
            screen.printf("Timeout");
            XFS_Speak("连接超时");
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
        XFS_Speak("连接成功");
    }

    ClearPage();
}

static void ButtonPressEvent()
{
    if(btOK || btEcd)
    {
        page.PageChangeTo(PAGE_CtrlInfo);
    }
    if(btBACK)
    {
        page.PageChangeTo(PAGE_MainMenu);
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

    HaveButtonEvent = true;
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

void PageRegister_Handshake(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
