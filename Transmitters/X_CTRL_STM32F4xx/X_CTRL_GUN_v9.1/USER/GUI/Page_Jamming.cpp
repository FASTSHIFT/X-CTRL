#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

const uint8_t JamChannelMax = 126;
uint8_t JamData[16];
int8_t JamDelay = 5;
uint8_t JamFreqStart = 0, JamFreqEnd = JamChannelMax / 2;
uint8_t JamFreq = JamFreqStart;

bool JamEnable = false;
bool JamEnableRandMode = false;
#define JamAdjValSelectMax 4
uint8_t JamAdjValSelect = 0;

#define JAB_WIDTH (JamChannelMax + 2)
LightGUI::ScrollBar<SCREEN_CLASS> JamAreaBar(
    &screen,
    /*x, y*/
    (screen.width() - JAB_WIDTH) / 2, ScreenMid_H,
    /*w, h*/
    JAB_WIDTH, 10,
    /*dir, l*/
    0, JamChannelMax / 2
);

static void JamFreqProcess()
{
    JamFreqStart = (JamAreaBar.Width - JamAreaBar.Long - 2) * (JamAreaBar.Scroll / 1000.0) + 1;
    JamFreqEnd = JamFreqStart + JamAreaBar.Long;

    if(!JamEnable)
        return;
    
    randomSeed(RNG_GetRandNum());
    
    for(int i = 0; i < __Sizeof(JamData); i++)
    {
        JamData[i] = random(0, 0xFF);
    }

    if(JamEnableRandMode)
    {
        JamFreq = random(JamFreqStart, JamFreqEnd);
    }
    else
    {
        JamFreq++;
        if(JamFreq > JamFreqEnd)
            JamFreq = JamFreqStart;
    }

    nrf.TranCheck();
    nrf.SetChannel(JamFreq);
    nrf.Tran(JamData);
}

static void JamSetDelay(uint8_t delayMs)
{
    __LimitValue(delayMs, 1, 20);
    JamDelay = delayMs;
#ifdef TIM_Handshake
    Timer_SetInterruptTimeUpdate(TIM_Handshake, JamDelay * 1000);
#endif
}

static void JamRFPrepare(bool setup)
{
    static uint8_t
    speed, channel,
           autoRetryDelay, autoRetryCount,
           txPloadWidth,   rxPloadWidth
           ;

    if(setup)
    {
        channel = nrf.GetFreqency();
        speed = nrf.GetSpeed();
        autoRetryDelay = nrf.RF_AutoRetryDelay;
        autoRetryCount = nrf.RF_AutoRetryCount;
        txPloadWidth = nrf.RF_TX_PloadWidth;
        rxPloadWidth = nrf.RF_RX_PloadWidth;

        nrf.SetRF_Enable(false);
        nrf.SetSpeed(nrf.SPEED_2Mbps);
        nrf.SetFreqency(0);
        nrf.SetAutoRetry(0, 15);
        nrf.SetPayloadWidth(sizeof(JamData), sizeof(JamData));
        nrf.TX_Mode();
        nrf.SetRF_Enable(true);

#ifdef TIM_Handshake
        Timer_SetInterrupt(
            TIM_Handshake,
            JamDelay * 1000,
            JamFreqProcess
        );
        TIM_Cmd(TIM_Handshake, ENABLE);
#endif
    }
    else
    {
        nrf.SetRF_Enable(false);
        nrf.SetSpeed(speed);
        nrf.SetFreqency(channel);
        nrf.SetAutoRetry(autoRetryDelay, autoRetryCount);
        nrf.SetPayloadWidth(txPloadWidth, rxPloadWidth);

        JamEnable = false;
#ifdef TIM_Handshake
        TIM_Cmd(TIM_Handshake, DISABLE);
#endif
    }
}

static bool ReqClearFreqLine = false;
static void JamDrawFreqLineProcess()
{
#define TopLimit (StatusBar_POS + 2)
#define HeightLimit (JamAreaBar.Y - TopLimit - 1)
    if(ReqClearFreqLine)
    {
        screen.fillRect(
            JamAreaBar.X, TopLimit,
            JamAreaBar.Width, HeightLimit,
            screen.Black
        );
        ReqClearFreqLine = false;
    }

    if(!JamEnable)
        return;

    static uint16_t lineColor;
    screen.drawFastVLine(JamAreaBar.X + JamFreq, TopLimit, HeightLimit, lineColor += 19);
}

static void JamDisplayProcess()
{
    JamAreaBar.Color_FM = screen.White;
    JamAreaBar.Color_SB = screen.Red;
    uint16_t textColor = screen.White;

    if(JamAdjValSelect == 1)
    {
        JamAreaBar.Color_FM = screen.Yellow;
    }
    else if(JamAdjValSelect == 2)
    {
        JamAreaBar.Color_SB = screen.Yellow;
    }
    else if(JamAdjValSelect == 3)
    {
        textColor = screen.Yellow;
    }
    JamAreaBar.display();
    screen.setTextColor(textColor, screen.Black);
#define DISP_TEXT_BASE (JamAreaBar.Y + JamAreaBar.Height + 20)
#define DISP_TEXT_HEIGHT (TEXT_HEIGHT_1 + 2)
    screen.setCursor(JamAreaBar.X + 1, DISP_TEXT_BASE);
    screen.printf("Jamming:%2dms %s", JamDelay, JamEnable ? "ON " : "OFF");

    screen.setTextColor(
        (JamAdjValSelect == 1 || JamAdjValSelect == 2) ? screen.Yellow : screen.White,
        screen.Black
    );
    screen.setCursor(JamAreaBar.X + 1, DISP_TEXT_BASE + DISP_TEXT_HEIGHT);
    screen.printf("Freq:%4d - %4d MHz", 2400 + JamFreqStart, 2400 + JamFreqEnd);

    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(JamAreaBar.X + 1, DISP_TEXT_BASE + DISP_TEXT_HEIGHT * 2);
    screen.printf("Mode:%s", JamEnableRandMode ? "Random" : "Order ");
}

static void JamAdjValProcess(int8_t adj)
{
    if(JamAdjValSelect == 1)
    {
        JamAreaBar.Scroll += (adj * 10);
        __LimitValue(JamAreaBar.Scroll, 0, 1000);
        ReqClearFreqLine = true;
    }
    else if(JamAdjValSelect == 2)
    {
        JamAreaBar.Long += adj;
        __LimitValue(JamAreaBar.Long, 5, JamAreaBar.Width - 2);
        ReqClearFreqLine = true;
    }
    else if(JamAdjValSelect == 3)
    {
        JamSetDelay(JamDelay + adj);
    }
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    JamRFPrepare(true);
    Draw_RFScaleplate(
        JamAreaBar.X,
        JamAreaBar.Y + JamAreaBar.Height + 1,
        JamChannelMax,
        screen.White
    );
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
#ifndef TIM_Handshake
    __IntervalExecute(JamFreqProcess(), JamDelay);
#endif
    __IntervalExecute(JamDisplayProcess(), 30);
    __IntervalExecute(JamDrawFreqLineProcess(), 20);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    JamRFPrepare(false);
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    //Serial.printf("event %d, obj:%d\r\n",event,(int)param);
    if(param == &btOK)
    {
        if(event == EVENT_ButtonPress)
        {
            JamAdjValSelect++;
            JamAdjValSelect %= JamAdjValSelectMax;
        }
        else if(event == EVENT_ButtonLongPressed)
        {
            JamEnable = !JamEnable;
            ReqClearFreqLine = true;
        }
    }
    else if(param == &btBACK)
    {
        if(event == EVENT_ButtonPress)
        {
            JamEnableRandMode = !JamEnableRandMode;
        }
        else if(event == EVENT_ButtonLongPressed)
        {
            page.PagePop();
        }
    }

    if(JamAdjValSelect == 0)
        return;

    if(param == &btUP)
    {
        if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
        {
            JamAdjValProcess(+1);
        }
    }
    else if(param == &btDOWN)
    {
        if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
        {
            JamAdjValProcess(-1);
        }
    }
}

/**
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Jamming(uint8_t ThisPage)
{
    page.PageRegister(ThisPage, Setup, Loop, Exit, Event);
}
