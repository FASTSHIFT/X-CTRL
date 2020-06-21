#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

#define NoOperationTimeout (60*1000)

bool Lock_CommonPassback;
static MillisTaskManager mtm_CtrlInfo(4);
static LightGUI::ProgressBar<SCREEN_CLASS> Throttle(&screen, 0);
static LightGUI::ProgressBar<SCREEN_CLASS> Brake(&screen, 0);
static bool EnableCruiseControl = false;
bool EnableDisplay = true;
static uint8_t LimitSelect = 0;

static uint32_t SetupTimePoint;
static uint32_t ButtonPressTimePoint;
static uint32_t JoystickMoveTimePoint;

static void Task_SignalMonitor()
{
    if(millis() - SetupTimePoint > 5000)
        Task_BuzzSignalMonitor();
}

static void Task_AutoDisplayOff()
{
    if(millis() - ButtonPressTimePoint > 10 * 1000)
        EnableDisplay = false;
    else
        EnableDisplay = true;

    __ValueMonitor(
        EnableDisplay,
        (
            TIM_Cmd(TIM_DisplayThread, (FunctionalState)EnableDisplay),
            EnableDisplay ? screen.ssd1306_command(SSD1306_DISPLAYON) : screen.ssd1306_command(SSD1306_DISPLAYOFF)
        )
    );

    if(ABS(JS_L.Y) > 100)
    {
        JoystickMoveTimePoint = millis();
    }

    if(!EnableCruiseControl && millis() - JoystickMoveTimePoint > NoOperationTimeout && millis() - ButtonPressTimePoint > NoOperationTimeout)
    {
        __IntervalExecute(BuzzMusic(5), 5000);
    }
}

static float throttle, brake, limit_l, limit_r;

static void DisplayCommonInfoIn(int16_t pos)
{
    screen.setCursor(StatusBarPos + pos, 1);
    screen.printf("THR:%3d%%", (int)(throttle * 100));
    screen.setCursor(StatusBarPos + pos, 10);
    screen.printf("BRK:%3d%%", (int)(brake * 100));
    Throttle.setProgress(throttle);
    Brake.setProgress(brake);

    if(LimitSelect == 1)
        screen.setTextColor(screen.Black, screen.White);
    else
        screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(StatusBarPos + pos, 20);
    screen.printf("SPD%3d%%", (int)(limit_l * 100));

    if(LimitSelect == 2)
        screen.setTextColor(screen.Black, screen.White);
    else
        screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(StatusBarPos + 45 + pos, 20);
    screen.printf("ACC%3d%%", (int)(limit_r * 100));

    screen.setTextColor(screen.White, screen.Black);
}

static void Task_CommonDisplay()
{
    limit_l = CTRL.KnobLimit.L / 1000.0;
    limit_r = CTRL.KnobLimit.R / 1000.0;

    if(EnableCruiseControl)
    {
        if(JS_L.Y < -100)
            EnableCruiseControl = false;
    }
    else
    {
        CTRL.Left.Y = JS_L.Y;
    }

    if(CTRL.Left.Y > 0)
    {
        throttle = __Map(CTRL.Left.Y, 0, CtrlOutput_MaxValue, 0.0, 1.005) * limit_l;
        brake = 0.0;
    }
    else
    {
        throttle = 0.0;
        brake = __Map(ABS(CTRL.Left.Y), 0, CtrlOutput_MaxValue, 0.0, 1.005);
    }

    DisplayCommonInfoIn(3);
}

static void Setup()
{
    ClearPage();
    ButtonPressTimePoint = SetupTimePoint = millis();
    State_RF = ON;
    mtm_CtrlInfo.TaskRegister(0, Task_CommonDisplay, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_SignalMonitor, 20);
    mtm_CtrlInfo.TaskRegister(3, Task_AutoDisplayOff, 100);

    Throttle.X = StatusBarPos + 55;
    Throttle.Y = 1;
    Throttle.Height = 8;
    Throttle.Width = 35;
    Throttle.display();

    Brake.X = Throttle.X;
    Brake.Y = Throttle.Y + Throttle.Height + 1;
    Brake.Height = Throttle.Height;
    Brake.Width = Throttle.Width;
    Brake.display();
}

static void ButtonUpDown_Handler()
{
    if(btOK)
    {
        LimitSelect ++;
        LimitSelect %= 3;
    }

    if(btUP)
    {
        if(LimitSelect == 1)
            CTRL.KnobLimit.L += 50;
        else if(LimitSelect == 2)
            CTRL.KnobLimit.R += 50;
    }
    if(btDOWN)
    {
        if(LimitSelect == 1)
            CTRL.KnobLimit.L -= 50;
        else if(LimitSelect == 2)
            CTRL.KnobLimit.R -= 50;
    }

    __ValueMonitor(CTRL.KnobLimit.L, BuzzTone(CTRL.KnobLimit.L + 500, 20));
    __ValueMonitor(CTRL.KnobLimit.R, BuzzTone(CTRL.KnobLimit.R + 500, 20));
    __LimitValue(CTRL.KnobLimit.L, 0, CtrlOutput_MaxValue);
    __LimitValue(CTRL.KnobLimit.R, 0, CtrlOutput_MaxValue);
}

static void Loop()
{
    mtm_CtrlInfo.Running(millis());
}

static void Exit()
{
}

static void ButtonPressEvent()
{
    ButtonPressTimePoint = millis();

    if(btOK)
        EnableCruiseControl = false;

    if(!EnableDisplay)
        return;

    ButtonUpDown_Handler();
}

static void ButtonLongPressEvent()
{
    if(btOK == btOK.LongPress)
    {
        __IntervalExecute(BuzzTone(1000, 50), 100);
        EnableCruiseControl = true;
    }

    if(btDOWN == btDOWN.LongPress && LimitSelect == 0)
    {
        __IntervalExecute(
            (BuzzTone(2000, 500), EEPROM_Handle(EEPROM_Chs::SaveData)),
            5000
        );
    }
    if(btUP == btUP.LongPress && LimitSelect == 0 && !EnableCruiseControl)
    {
        page.PageChangeTo(PAGE_Dashboard);
    }

    __IntervalExecute(ButtonUpDown_Handler(), 100);
}

void PageRegister_CtrlInfo(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
