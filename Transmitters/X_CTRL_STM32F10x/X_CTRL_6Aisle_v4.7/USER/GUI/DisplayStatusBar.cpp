#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*状态栏时间片调度器，开启优先级*/
static MillisTaskManager mtm_StatusBar(3, true);

/*电池电量显示控件*/
static LightGUI::ProgressBar<SCREEN_CLASS> PB_Batt(&screen, 0);

bool State_DisplayCPU_Usage = false;

/**
  * @brief  电池电量显示更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawBattUsage()
{
    static uint8_t usage;//显示的电池电量
    
    __ValueCloseTo(usage, BattUsage, 1);//将显示的电量靠近实际的电量，达到动画的效果

    if(usage < 30)
        PB_Batt.Color_PB = screen.Red;//电量<30%，电池为红色
    else if(usage >= 90)
        PB_Batt.Color_PB = screen.Green;//电量>90%，电池为绿色
    else
        PB_Batt.Color_PB = screen.White;//电量正常，电池为白色

    PB_Batt.setProgress(usage / 100.0);//更新控件
}

/**
  * @brief  NRF收发状态显示任务
  * @param  无
  * @retval 无
  */
static void Task_DrawNrfTxRxState()
{   
    screen.fillRect(69, 2, 5, 7, screen.Black);
    if(page.NowPage == PAGE_CtrlInfo)
    {
        //screen.drawFastVLine(71, 2, 7, screen.White);       
        if(nrf.GetTxRxState() == nrf.RX_MODE)
        {
            screen.drawLine(71, 8, 71-2, 8-2, screen.White);
            screen.drawLine(71, 8, 71+2, 8-2, screen.White);
        }
        else //(nrf.GetTxRxState() == nrf.TX_MODE)
        {     
            screen.drawLine(71, 2, 71-2, 2+2, screen.White);
            screen.drawLine(71, 2, 71+2, 2+2, screen.White);
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

    screen.setCursor(2, 2);
    screen.printf("%3d%%", NRF_SignalStrength);

    /* 射频通信状态 */
    if(State_RF)
    {
        if(!digitalRead(SPDT_Switch_Pin))
            screen.setTextColor(screen.Yellow, screen.Black);
        else
            screen.setTextColor(screen.Green, screen.Black);
    }
    else
    {
        screen.setTextColor(screen.Black, screen.Black);
    }
    screen.setCursor(56, 2);
    screen.print("RF");

    /* 显示电池电压 */
    screen.setCursor(80, 2);
    screen.setTextSize(0);
    screen.setTextColor(screen.Blue, screen.Black);
    screen.printf("%1.1fV", BattVoltage);

    /* 开机一段时间后降低电池电量刷新率 */
    if(millis() > 3000)
        mtm_StatusBar.TaskSetIntervalTime((uint8_t)0, 100);
}

/**
  * @brief  初始化状态栏
  * @param  无
  * @retval 无
  */
void Init_StatusBar()
{
    /*状态栏合拢动画*/
    for(float i = 0.0; i < 1.0; i += 0.001)
    {
        screen.drawFastHLine(0, StatusBar_POS, screen.width() / 2 * i, screen.White);
        screen.drawFastHLine(screen.width() - screen.width() / 2 * i - 1, StatusBar_POS, screen.width() / 2 * i, screen.White);
    }

    /*电池电量显示控件属性设置*/
    PB_Batt.X = 105;
    PB_Batt.Y = 1;
    PB_Batt.Width = 20;
    PB_Batt.Height = 10;
    screen.drawFastVLine(
        PB_Batt.X + PB_Batt.Width,
        PB_Batt.Y + 2,
        PB_Batt.Height - 4,
        screen.White
    );

    /*任务注册*/
    mtm_StatusBar.TaskRegister(0, Task_DrawBattUsage, 5);
    mtm_StatusBar.TaskRegister(1, Task_1000msUpdate, 1000);
    mtm_StatusBar.TaskRegister(2, Task_DrawNrfTxRxState, 123);
    
    Task_1000msUpdate();
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
