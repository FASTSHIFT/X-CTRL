#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"
#include "math.h"

/*选项起始坐标*/
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14

#define TextHeight 16

/*被选中的选项*/
static int16_t ItemSelect = 0;

/*选项更新标志位*/
static bool ItemSelectUpdating = true;

/*可选选项最大个数*/
static uint8_t ItemSelect_MAX = 0;

/*强制跳出握手过程*/
static bool ExitHandshake = false;

/*是否有按键事件发生*/
static bool HaveButtonEvent = false;

/**
  * @brief  更新选项字符串
  * @param  无
  * @retval 无
  */
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

        /*显示从机ID和文本描述*/
        screen.setCursor(ItemStartX, ItemStartY + i * TextHeight);
        screen.printfX("ID:0x%x", HandshakePack_Slave[i].ID);
        screen.printfX(" %s", HandshakePack_Slave[i].Description);
    }
}

/*映射*/
static float LinearMap(float value, float oriMin, float oriMax, float nmin, float nmax)
{
    float k = 0;
    if (oriMax != oriMin)
    {
        k = (value - oriMin) / (oriMax - oriMin);
    }
    return nmin + k * (nmax - nmin);
}

static float PowMap(float value, float omin, float omax, float mmin, float mmax, float stK, float edK)
{
    if (stK == edK)
    {
        return LinearMap(value, omin, omax, mmin, mmax);
    }
    else
    {
        float stX = (stK);
        float edX = (edK);
        float stY = exp(stX);
        float edY = exp(edX);
        float x = LinearMap(value, omin, omax, stX, edX);
        return LinearMap(exp(x), stY, edY, mmin, mmax);
    }
}


/*实例化光标控件对象*/
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_1(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_2(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_3(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_4(&screen, 0, screen.height() - 20, 4, 4);
static LightGUI::Cursor<SCREEN_CLASS> ProgressCursor_5(&screen, 0, screen.height() - 20, 4, 4);

static float Animation_ProgressSlide(float from, float to, uint32_t ms, uint16_t T, uint16_t beginTime = 0)
{
    const uint16_t a1width = 700;
    const uint16_t a1begin = 500 - a1width / 2;
    const uint16_t a1end = 500 + a1width / 2;
    const float stK = 2.5f;
    const float edK = 1.2f;
    ms = ms % (T + beginTime);
    if (ms > beginTime)
    {
        ms -= beginTime;
    }
    else
    {
        return -99;
    }
    uint16_t pos = LinearMap(ms, 0, T, 0, 1000);
    float value = 0;
    if (pos < a1begin)
    {
        value = PowMap(pos, 0, a1begin, from, from + 0.3f * (to - from), stK, edK);
    }
    else if (pos < a1end)
    {
        value = LinearMap(pos - a1begin, 0, a1width, from + 0.3f * (to - from), from + 0.7f * (to - from));
    }
    else
    {
        value = PowMap(pos - a1end, 0, a1begin, from + 0.7f * (to - from), to, edK, stK);
    }
    return value;
}

void SearchingAnimation(uint32_t ms)
{
    uint16_t bgtime = 200;
    float pos1 = Animation_ProgressSlide(0, screen.width(), ms, 2000, 2000);
    float pos2 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 1, 2000, 2000);
    float pos3 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 2, 2000, 2000);
    float pos4 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 3, 2000, 2000);
    float pos5 = Animation_ProgressSlide(0, screen.width(), ms + bgtime * 4, 2000, 2000);

    screen.fillRect(0, ProgressCursor_1.Y, 1, 4, screen.Black);

    ProgressCursor_1.setPosition(pos1, ProgressCursor_1.Y);
    ProgressCursor_2.setPosition(pos2, ProgressCursor_2.Y);
    ProgressCursor_3.setPosition(pos3, ProgressCursor_3.Y);
    ProgressCursor_4.setPosition(pos4, ProgressCursor_4.Y);
    ProgressCursor_5.setPosition(pos5, ProgressCursor_5.Y);
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
    LightGUI::ProgressBar<SCREEN_CLASS> SearchProgress(&screen, 0, screen.height() - 20, screen.width(), 10, 0);
    SearchProgress.Color_FM = screen.Black;

    screen.setCursor(ItemStartX, ItemStartY);
    screen.setTextColor(screen.White, screen.Black);
    screen.printfX("Searching...");
    XFS_Speak("正在搜索");

    /*超时设置5000ms*/
    uint32_t time = millis();
    while(millis() - time < 12000)
    {
        /*获取从机列表数量*/
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);

        if(ItemSelect_MAX)
        {
            screen.setCursor(ItemStartX, ItemStartY + TextHeight);
            screen.setTextColor(screen.White, screen.Black);
            screen.printfX("Find %d Slave...", ItemSelect_MAX);
        }

        /*滚动动画*/
//      if(millis() / 400 % 2)
//      {
//          SearchProgress.Color_BG = screen.Black;
//          SearchProgress.Color_PB = screen.White;
//      }
//      else
//      {
//          SearchProgress.Color_PB = screen.Black;
//          SearchProgress.Color_BG = screen.White;
//      }
//        SearchProgress.setProgress((millis() % 400) / 400.0);
        SearchingAnimation(millis() - time);

        PageDelay(1);

        /*是否强制跳出握手过程*/
        if(HaveButtonEvent) break;
    }

    /*显示搜索结果*/
    screen.setCursor(ItemStartX, ItemStartY + 2 * TextHeight);
    if(ItemSelect_MAX > 0)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.printfX("Search Done!");
        XFS_Speak("搜索完成,找到 %d 个设备", ItemSelect_MAX);
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.printfX("Not Found!");
        XFS_Speak("未找到任何设备");
        ExitHandshake = true;
    }
    PageDelay(200);

    ClearPage();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
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

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
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

    /*尝试连接从机*/
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(ItemStartX, ItemStartY);
    screen.printfX(HandshakePack_Slave[ItemSelect].Description);
    screen.setCursor(ItemStartX, ItemStartY + TextHeight);
    screen.printfX("Connecting...");
    XFS_Speak("正在尝试连接到%s", HandshakePack_Slave[ItemSelect].Description);
    /*超时设置*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*等待从机响应握手信号*/
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        /*2500ms超时*/
        if(millis() - timeout > 2500)
        {
            screen.setTextColor(screen.Red, screen.Black);
            screen.setCursor(ItemStartX, ItemStartY + TextHeight);
            screen.printfX("Timeout");
            XFS_Speak("连接超时");
            IsTimeout = true;
            break;
        }
        PageDelay(1);
    }

    /*握手收尾设置，跳转至约定好的握手频率以及地址*/
    HandshakeRun(HandshakeState_Connected);

    /*对应从机类型*/
    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;

    /*如果未超时表示握手成功*/
    if(!IsTimeout)
    {
        screen.setTextColor(screen.Green, screen.Black);
        screen.setCursor(ItemStartX, ItemStartY + TextHeight);
        screen.printfX("Success");
        XFS_Speak("连接成功");
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

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
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

/**
  * @brief  握手页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Handshake(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
