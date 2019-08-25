#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

static MillisTaskManager mtm_StatusBar(3, true);
static LightGUI::ProgressBar<SCREEN_CLASS> PB_Batt(&screen, 0);

#define TextPosBase_Y (StatusBar_POS-10)

static void Task_DrawBattUsage()
{
    static uint8_t usage;

    if(usage < (uint8_t)BattUsage)
        usage++;
    else if(usage > (uint8_t)BattUsage)
        usage--;

    if(usage < 30)
        PB_Batt.Color_PB = screen.Red;
    else if(usage >= 90)
        PB_Batt.Color_PB = screen.Green;
    else
        PB_Batt.Color_PB = screen.White;

    PB_Batt.setProgress(usage / 100.0);
}

static void Task_DrawNrfTxRxState()
{
    screen.fillRect(69, 2, 5, 7, screen.Black);
    if(page.NowPage == PAGE_CtrlInfo)
    {
        //screen.drawFastVLine(71, 2, 7, screen.White);
        if(nrf.GetTxRxState() == nrf.TX_MODE)
        {     
            screen.drawLine(71, 2, 71-2, 2+2, screen.White);
            screen.drawLine(71, 2, 71+2, 2+2, screen.White);
        }
        else
        {
            screen.drawLine(71, 8, 71-2, 8-2, screen.White);
            screen.drawLine(71, 8, 71+2, 8-2, screen.White);
        }
    }
}

static void Task_1000msUpdate()
{
    /* NRF 信号强度(0~100%) */
    NRF_SignalStrength = nrf.GetSignalStrength();
    screen.setTextSize(0);
    if(NRF_SignalStrength >= 90)
        screen.setTextColor(screen.Green, screen.Black);
    else if(NRF_SignalStrength >= 60)
        screen.setTextColor(screen.White, screen.Black);
    else if(NRF_SignalStrength >= 20)
        screen.setTextColor(screen.Yellow, screen.Black);
    else if(NRF_SignalStrength < 20)
        screen.setTextColor(screen.Red, screen.Black);

    screen.setCursor(2, TextPosBase_Y);
    screen.printf("%3d%%", NRF_SignalStrength);

    /* 显示蓝牙连接状态 */
    screen.setCursor(33, TextPosBase_Y);
    if(State_Bluetooth)
    {
        if(hc05.GetState())
            screen.setTextColor(screen.Green, screen.Black);
        else
            screen.setTextColor(screen.Yellow, screen.Black);

        if(Bluetooth_ConnectObject == BC_Type::BC_HMI)
            screen.print("HMI");
        else if(Bluetooth_ConnectObject == BC_Type::BC_XFS)
            screen.print("XFS");
    }
    else
        screen.print("   ");

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
        screen.setTextColor(screen.Red, screen.Black);
    }
    screen.setCursor(screen.width() / 2, TextPosBase_Y);
    screen.print("RF");

    /* 显示电池电压 */
    screen.setCursor(PB_Batt.X - 4 * 8 - 2, TextPosBase_Y);
    screen.setTextSize(1);
    screen.setTextColor(screen.Blue, screen.Black);
    screen.printf("%1.1fV", BattVoltage);

    /* 开机一段时间后降低电池电量刷新率 */
    if(millis() > 3000)
        mtm_StatusBar.TaskSetIntervalTime((uint8_t)0, 500);
}

void Init_StatusBar()
{
    for(float i = 0.0; i < 1.0; i += 0.001)
    {
        screen.drawFastHLine(0, StatusBar_POS, screen.width() / 2 * i, screen.White);
        screen.drawFastHLine(screen.width() - screen.width() / 2 * i - 1, StatusBar_POS, screen.width() / 2 * i, screen.White);
    }

    PB_Batt.Width = 20;
    PB_Batt.Height = 10;
    PB_Batt.X = screen.width()-PB_Batt.Width - 3;
    PB_Batt.Y = TextPosBase_Y - 1;
    screen.drawFastVLine(
        PB_Batt.X + PB_Batt.Width,
        PB_Batt.Y + 2,
        PB_Batt.Height - 4,
        screen.White
    );

    mtm_StatusBar.TaskRegister(0, Task_DrawBattUsage, 5);
    mtm_StatusBar.TaskRegister(1, Task_1000msUpdate, 1000);
    mtm_StatusBar.TaskRegister(2, Task_DrawNrfTxRxState, 123);
}

void Thread_StatusBar()
{
    mtm_StatusBar.Running(millis());
}
