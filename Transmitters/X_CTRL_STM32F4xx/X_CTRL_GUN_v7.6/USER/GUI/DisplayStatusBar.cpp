#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*状态栏时间片调度器，开启优先级*/
static MillisTaskManager mtm_StatusBar(3, true);

/*电池电量显示控件*/
static LightGUI::ProgressBar<SCREEN_CLASS> PB_Batt(&screen, 0);

/*CPU占用率显示使能*/
bool State_DisplayCPU_Usage = false;

/*文本Y轴基坐标*/
#define TextPosBase_Y (StatusBar_POS-15)

/**
  * @brief  电池电量显示更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawBattUsage()
{
    static uint8_t usage;//显示的电池电量
    bool IsBattCharging = !digitalRead(BattCharge_Pin);

    __ValueCloseTo(usage, (int)BattUsage, 1);//将显示的电量靠近实际的电量，达到动画的效果
    if(IsBattCharging && usage == (uint8_t)BattUsage)
    {
        usage = 0;
    }

    __ValueMonitor(
        IsBattCharging,
        IsBattCharging ? BuzzMusic(MC_Type::MC_BattChargeStart) : BuzzMusic(MC_Type::MC_BattChanargeEnd)
    );

    /*电量显示图标*/
    if(usage < 30)
        PB_Batt.Color_PB = screen.Red;//电量<30%，电池为红色
    else if(usage >= 90)
        PB_Batt.Color_PB = screen.Green;//电量>90%，电池为绿色
    else
        PB_Batt.Color_PB = screen.White;//电量正常，电池为白色

    PB_Batt.setProgress(usage / 100.0);//更新控件

    /*电池头*/
    screen.drawFastVLine(
        PB_Batt.X + PB_Batt.Width,
        PB_Batt.Y + 2,
        PB_Batt.Height - 4,
        screen.White
    );
}

/**
  * @brief  NRF收发状态显示任务
  * @param  无
  * @retval 无
  */
static void Task_DrawNrfTxRxState()
{
    const int16_t PosBase = screen.width() / 2 + 5;
    screen.fillRect(PosBase, StatusBar_POS - 10, 5, 7, screen.Black);
    if(page.NowPage == PAGE_CtrlInfo)
    {
        //screen.drawFastVLine(PosBase + 2, 2, 7, screen.White);
        if(nrf.GetTxRxState() == nrf.RX_MODE)
        {
            screen.drawLine(PosBase + 2, StatusBar_POS - 4, PosBase + 2 - 2, StatusBar_POS - 4 - 2, screen.White);
            screen.drawLine(PosBase + 2, StatusBar_POS - 4, PosBase + 2 + 2, StatusBar_POS - 4 - 2, screen.White);
        }
        else //(nrf.GetTxRxState() == nrf.TX_MODE)
        {
            screen.drawLine(PosBase + 2, StatusBar_POS - 10, PosBase + 2 - 2, StatusBar_POS - 10 + 2, screen.White);
            screen.drawLine(PosBase + 2, StatusBar_POS - 10, PosBase + 2 + 2, StatusBar_POS - 10 + 2, screen.White);
        }
    }
}

/**
  * @brief  1000ms更新任务
  * @param  无
  * @retval 无
  */
static void Task_1000msUpdate()
{
    if(State_DisplayCPU_Usage)
    {
        if(CPU_Usage > 90.0f)
            screen.setTextColor(screen.Red, screen.Black);
        else if(CPU_Usage > 50.0f)
            screen.setTextColor(screen.Yellow, screen.Black);
        else if(CPU_Usage > 10.0f)
            screen.setTextColor(screen.White, screen.Black);
        else
            screen.setTextColor(screen.Green, screen.Black);

        screen.setCursor(10, StatusBar_POS + 2);
        screen.printfX("CPU:%0.2f%% %0.1fC", CPU_Usage, CPU_Temperature);
    }

    /* NRF 信号强度(0~100%) */
    NRF_SignalStrength = nrf.GetSignalStrength();
    screen.setTextSize(1);
    if(NRF_SignalStrength >= 90)
        screen.setTextColor(screen.Green, screen.Black);
    else if(NRF_SignalStrength >= 60)
        screen.setTextColor(screen.White, screen.Black);
    else if(NRF_SignalStrength >= 20)
        screen.setTextColor(screen.Yellow, screen.Black);
    else if(NRF_SignalStrength < 20)
        screen.setTextColor(screen.Red, screen.Black);

    screen.setCursor(2, TextPosBase_Y);
    screen.printfX("%3d%%   ", NRF_SignalStrength);

    /* 显示蓝牙连接状态 */
    screen.setCursor(33, TextPosBase_Y);
    if(State_Bluetooth)
    {
        if(hc05.GetState())
            screen.setTextColor(screen.Green, screen.Black);
        else
            screen.setTextColor(screen.Yellow, screen.Black);

        screen.printfX(StrBtc[Bluetooth_ConnectObject]);
    }
    else
        screen.fillRect(33, StatusBar_POS - 16, 22, 16, screen.Black);

    /* 射频通信状态 */
    if(State_RF)
    {
        if(IS_EnableCtrl())
            screen.setTextColor(screen.Green, screen.Black);
        else
            screen.setTextColor(screen.Yellow, screen.Black);
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
    }
    screen.setCursor(screen.width() / 2 - 9, TextPosBase_Y);
    screen.printfX("RF");

    /* 显示电池电压 */
    screen.setCursor(PB_Batt.X - 27, TextPosBase_Y);
    screen.setTextSize(0);
    screen.setTextColor(screen.White, screen.Black);
    screen.printfX("%1.1fV", BattVoltage);
}

/**
  * @brief  初始化状态栏
  * @param  无
  * @retval 无
  */
void Init_StatusBar()
{
    /*状态栏合拢动画*/
    for(float i = 0.0f; i < 1.0f; i += 0.001f)
    {
        screen.drawFastHLine(0, StatusBar_POS, screen.width() / 2 * i + 1, screen.White);
        screen.drawFastHLine(screen.width() - screen.width() / 2 * i, StatusBar_POS, screen.width() / 2 * i + 1, screen.White);
    }

    pinMode(BattCharge_Pin, INPUT_PULLUP);

    /*电池电量显示控件属性设置*/
    PB_Batt.Width = 20;
    PB_Batt.Height = 10;
    PB_Batt.X = screen.width() - PB_Batt.Width - 5;
    PB_Batt.Y = StatusBar_POS - PB_Batt.Height - 2;

    /*任务注册*/
    mtm_StatusBar.TaskRegister(0, Task_DrawBattUsage, 20);
    mtm_StatusBar.TaskRegister(1, Task_1000msUpdate, 1000);
    mtm_StatusBar.TaskRegister(2, Task_DrawNrfTxRxState, 123);
}

/**
  * @brief  状态栏线程
  * @param  无
  * @retval 无
  */
void Thread_StatusBar()
{
    mtm_StatusBar.Running(millis());
}
