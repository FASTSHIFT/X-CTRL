#include "FileGroup.h"
#include "GUI_Private.h"

MillisTaskManager mtm_CtrlInfo(4);

#define PBx_L 40
#define PBx_W 6

#define PBY_START (screen.width()/2-(PBx_W/2))
#define PBX_START (screen.width()/2-(PBx_L/2))

#define PBLY_END 0
#define PBRY_END (screen.width() - PBx_W)
#define PBLX_END 10
#define PBRX_END (screen.width() - 10 - PBx_L)

ScrollBar PB_LY(screen, PBY_START, StatusBar_POS + 4, PBx_W, PBx_L, 1, 4);
ScrollBar PB_RY(screen, PBY_START, StatusBar_POS + 4, PBx_W, PBx_L, 1, 4);
ScrollBar PB_LX(screen, PBX_START, screen.height() - PBx_W, PBx_L, PBx_W, 0, 4);
ScrollBar PB_RX(screen, PBX_START, screen.height() - PBx_W, PBx_L, PBx_W, 0, 4);

void ProgressBar_Open()
{
    for(double i = 0.0; i < 1.0; i += (((1.0 - i) * 0.1) + 0.001))
    {
        PB_LY.setPosition(PBY_START - i * (PBY_START - PBLY_END), PB_LY.Y);
        PB_RY.setPosition(PBY_START + i * (PBRY_END - PBY_START), PB_RY.Y);
        PB_LX.setPosition(PBX_START - i * (PBX_START - PBLX_END), PB_LX.Y);
        PB_RX.setPosition(PBX_START + i * (PBRX_END - PBX_START), PB_RX.Y);

        PB_LY.setProgress(i * 0.5);
        PB_RY.setProgress(i * 0.5);
        PB_LX.setProgress(1.0 - i * 0.5);
        PB_RX.setProgress(i * 0.5);
    }
}

void ProgressBar_Close()
{
    for(double i = 1.0; i > 0.0; i -= ((i * 0.1) + 0.001))
    {
        PB_LY.setPosition(PBY_START - i * (PBY_START - PBLY_END), PB_LY.Y);
        PB_RY.setPosition(PBY_START + i * (PBRY_END - PBY_START), PB_RY.Y);
        PB_LX.setPosition(PBX_START - i * (PBX_START - PBLX_END), PB_LX.Y);
        PB_RX.setPosition(PBX_START + i * (PBRX_END - PBX_START), PB_RX.Y);

        PB_LY.setProgress(i * 0.5);
        PB_RY.setProgress(i * 0.5);
    }
}

void Task_DrawProgress()
{
    PB_LX.setProgress(fmap(CTRL.Left.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.0));
    PB_LY.setProgress(fmap(CTRL.Left.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.0));
    PB_RX.setProgress(fmap(CTRL.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.0));
    PB_RY.setProgress(fmap(CTRL.Right.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0.0, 1.0));
}

void Task_PrintUseTime()
{
    screen.setCursor(70, 20);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    screen.printf("%02d:%02d:%02d", hh, mm, ss);
}

static float TopSpeed = 0.0;
void DisplayCarSSInfo()
{
    if(CarSS_Passback.Channel_1 >= 200.0)return;
    if(CarSS_Passback.Channel_1 > TopSpeed) TopSpeed = CarSS_Passback.Channel_1;

    screen.setCursor(60, 35);
    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(2);
    screen.printf("%5.1f", CarSS_Passback.Channel_1);

    screen.setCursor(17, 45);
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setTextSize(1);
    screen.printf("%5.1f", TopSpeed);
}

bool Is_CommonPassbackReading = false;
void DisplayCommonInfo()
{
    Is_CommonPassbackReading = true;
    static bool IsInit = false;
    if(!IsInit)
    {
        extern uint8_t NRF_RxBuff[32];
        Common_Passback.X = 70;
        Common_Passback.Y = 42;
        Common_Passback.TextSize = 1;
        Common_Passback.TextColor = screen.White;
        Common_Passback.BackColor = screen.Black;
        strcpy(Common_Passback.Text, (char*)"...");
        memcpy(NRF_RxBuff, &Common_Passback, sizeof(Common_Passback));
        IsInit = true;
    }
    else
    {
        if(Common_Passback.Y <= StatusBar_POS)return;
        screen.setCursor(Common_Passback.X, Common_Passback.Y);
        screen.setTextColor(Common_Passback.TextColor, Common_Passback.BackColor);
        screen.setTextSize(Common_Passback.TextSize);
        screen.print(Common_Passback.Text);
    }
    Is_CommonPassbackReading = false;
}

void Task_PrintPassback()
{
    if(PassBack_State)
    {
        if(CTRL.Info.CtrlObject == CAR_ServoSteering)
            DisplayCarSSInfo();
        else if(CTRL.Info.CtrlObject == X_COMMON)
            DisplayCommonInfo();
    }
}

static void Setup()
{
    PB_LX.Color_SB = screen.Yellow;
    PB_LY.Color_SB = screen.Yellow;
    PB_RX.Color_SB = screen.Yellow;
    PB_RY.Color_SB = screen.Yellow;
    ProgressBar_Open();

    uint16_t *gImage_Bitmap_x;

    if(CTRL.Info.CtrlObject == CAR_ServoSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_RC;
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_DS;
    else
        gImage_Bitmap_x = (uint16_t*)gImage_Bitmap_Common;

    FastDrawRGBBitmap(15, 15, gImage_Bitmap_x, 30, 30);

    mtm_CtrlInfo.TaskRegister(0, Task_DrawProgress, 20);
    mtm_CtrlInfo.TaskRegister(1, Task_PrintUseTime, 1000);
    mtm_CtrlInfo.TaskRegister(2, Task_PrintPassback, 30);
    mtm_CtrlInfo.TaskRegister(3, BuzzSignalMonitor, 20);
    RF_State = ON;
    TopSpeed = 0.0;
}

static void Loop()
{
    mtm_CtrlInfo.Running(millis());
}

static void Exit()
{
    ProgressBar_Close();
    screen.clearPage();
    RF_State = OFF;
}

static void ButtonPressEvent()
{
}

static void ButtonLongPressEvent()
{
    if(btEcd)
        pm.PageChangeTo(PAGE_MainMenu);
}

static void EncoderEvent()
{
}

void PageRegister_CtrlInfo()
{
#define ThisPage PAGE_CtrlInfo
    pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
    pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
    pm.PageRegister_EncoderEvent(ThisPage, EncoderEvent);
}
