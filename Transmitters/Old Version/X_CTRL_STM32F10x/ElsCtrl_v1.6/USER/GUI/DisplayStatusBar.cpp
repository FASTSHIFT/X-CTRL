#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

static MillisTaskManager mtm_StatusBar(3);
static LightGUI::ProgressBar<SCREEN_CLASS> Batt_Els(&screen, 0);
static LightGUI::ProgressBar<SCREEN_CLASS> Batt_Ctrl(&screen, 0);

#define FullVoltatge 16.8
#define EmptyVoltage 12.0
float ElsBattVoltage, ElsBattUsage;
float ElsSpeed;
float ElsDistance;
static uint32_t StatusBarOpenTime = 0;

#define State_BattVoltage (Common_Passback.X==13&&Common_Passback.Y==40)
#define State_ElsSpeed    (Common_Passback.X==80&&Common_Passback.Y==35)
#define State_ElsDistance (Common_Passback.X==80&&Common_Passback.Y==45)

static void Task_UpdateElsPassbackState()
{
    Lock_CommonPassback = true;
    if(State_BattVoltage)
    {
        float temp = String(Common_Passback.Text).toFloat();
        if(temp > 11.0 && temp < 17.0)
        {
            ElsBattVoltage = temp;
            ElsBattUsage = __Map(
                               ElsBattVoltage,
                               EmptyVoltage,
                               FullVoltatge,
                               0.0,
                               100.0
                           );
        }
    }
	else if(State_ElsSpeed)
	{
		ElsSpeed = String(Common_Passback.Text).toFloat();
	}
	else if(State_ElsDistance)
	{
		ElsDistance = String(Common_Passback.Text).toFloat() / 1000.0f;
	}
    Lock_CommonPassback = false;
}

static void DrawSignalStrength(uint8_t rssi)
{
    uint8_t c0, c1, c2, c3;

    if(rssi > 90)
    {
        c0 = screen.Black;
        c1 = screen.White;
        c2 = screen.White;
        c3 = screen.White;
    }
    else if(rssi > 50)
    {
        c0 = screen.Black;
        c1 = screen.White;
        c2 = screen.White;
        c3 = screen.Black;
    }
    else if(rssi > 30)
    {
        c0 = screen.Black;
        c1 = screen.White;
        c2 = screen.Black;
        c3 = screen.Black;
    }
    else
    {
        c0 = screen.White;
        c1 = screen.Black;
        c2 = screen.Black;
        c3 = screen.Black;
    }

    screen.drawLine(15, 4, 20, 9, c0);
    screen.drawLine(15, 9, 20, 4, c0);
    screen.drawFastVLine(15, 7, 3, c1);
    screen.drawFastVLine(15 + 2, 5, 5, c2);
    screen.drawFastVLine(15 + 4, 3, 7, c3);
    if(c0 == screen.White)
    {
        screen.drawLine(15, 4, 20, 9, c0);
        screen.drawLine(15, 9, 20, 4, c0);
    }

}

static void Task_DrawBattUsage()
{
    static uint8_t usageCtrl, usageEls;

    __ValueCloseTo(usageEls, (int)ElsBattUsage, 1);
    __ValueCloseTo(usageCtrl, (int)BattUsage, 1);

    if(digitalRead(ChargeDetect_Pin) == LOW && usageCtrl == (uint8_t)BattUsage)
    {
        usageCtrl = 0;
    }

    Batt_Els.setProgress(usageEls / 100.0);
    Batt_Ctrl.setProgress(usageCtrl / 100.0);
}

static void Task_1000msUpdate()
{
    NRF_SignalStrength = nrf.GetSignalStrength();
    DrawSignalStrength(NRF_SignalStrength);

    /* 开机一段时间后降低电池电量刷新率 */
    if(millis() > StatusBarOpenTime + 3000)
        mtm_StatusBar.TaskSetIntervalTime((uint8_t)0, 20);
}

#define ClearStatusBar(pos) screen.fillRect(0,0,pos,screen.height(),screen.Black)

static void DrawStatusBarIn(int16_t pos)
{
    /*画状态栏*/
    screen.drawFastVLine(pos, 0, screen.height(), screen.White);

    /*电池控件属性设置*/
    Batt_Els.X = pos - (StatusBarPos - 8);
    Batt_Els.Y = 12;
    Batt_Els.Width = 20;
    Batt_Els.Height = 8;
    Batt_Els.display();
    screen.drawFastVLine(
        Batt_Els.X + Batt_Els.Width,
        Batt_Els.Y + 2,
        Batt_Els.Height - 4,
        screen.White
    );

    Batt_Ctrl.X = Batt_Els.X;
    Batt_Ctrl.Y = Batt_Els.Y + Batt_Els.Height + 1;
    Batt_Ctrl.Width = Batt_Els.Width;
    Batt_Ctrl.Height = Batt_Els.Height;
    Batt_Ctrl.display();
    screen.drawFastVLine(
        Batt_Ctrl.X + Batt_Ctrl.Width,
        Batt_Ctrl.Y + 2,
        Batt_Ctrl.Height - 4,
        screen.White
    );

    /*电池标识*/
    screen.setCursor(pos - StatusBarPos, Batt_Els.Y + 1);
    screen.print('E');
    screen.setCursor(pos - StatusBarPos, Batt_Ctrl.Y + 1);
    screen.print('C');

    /*天线*/
    screen.drawFastVLine(pos - (StatusBarPos - 10), 0, 10, screen.White);
    screen.drawFastHLine(pos - (StatusBarPos - 5), 0, 11, screen.White);
    screen.drawLine(pos - (StatusBarPos - 5), 0, pos - (StatusBarPos - 10), 5, screen.White);
    screen.drawLine(pos - (StatusBarPos - 15), 0, pos - (StatusBarPos - 10), 5, screen.White);
}

void Init_StatusBar()
{
    TIM_Cmd(TIM_DisplayThread, DISABLE);

    LightGUI::Bitmap<SCREEN_CLASS> Logo(&screen, (uint8_t*)Bitmap_ElsLogo, screen.width(), 0, 100, 25);
    for(float x = 1; x > -1; x -= (GetNextPostion(x, 1.5, 0.05) * 0.05))
    {
        Logo.setPosition_f(x + 0.5, 0.5);
        screen.display();
        if(!digitalRead(Button_OK_Pin))
        {
            screen.clearDisplay();
            break;
        }
    }

    for(float i = 0.0; i < 1.0; i += ((1.1 - i) * 0.1))
    {
        ClearStatusBar(i * StatusBarPos);
        DrawStatusBarIn(i * StatusBarPos);
        screen.display();
        delay(10);
    }
    ClearStatusBar(StatusBarPos);
    DrawStatusBarIn(StatusBarPos);
    TIM_Cmd(TIM_DisplayThread, ENABLE);

    mtm_StatusBar.TaskRegister(0, Task_DrawBattUsage, 5);
	mtm_StatusBar.TaskRegister(1, Task_UpdateElsPassbackState, 200);
	mtm_StatusBar.TaskRegister(2, Task_1000msUpdate, 1000);
    StatusBarOpenTime = millis();
}

void Thread_StatusBar()
{
    mtm_StatusBar.Running(millis());
}
