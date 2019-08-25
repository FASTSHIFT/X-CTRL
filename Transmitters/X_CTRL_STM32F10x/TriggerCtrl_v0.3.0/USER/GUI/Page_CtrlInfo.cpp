#include "FileGroup.h"
#include "GUI_Widget.h"

MillisTaskManager mtmCtrlInfo(5);

static JoystickPos JSL(tft, 4, 68, 58, 58, 5);
static JoystickPos JSR(tft, 100, 68, 58, 58, 5);
static ScrollBar KnobLimitL(tft, 62, 68, 7, 58, 1, 10);
static ScrollBar KnobLimitR(tft, 91, 68, 7, 58, 1, 10);
static JoystickPos MPU(tft,100,4,58,58,5);
void Task_UpdateJsPos()
{
    float lx = fmap(JS_L.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0);
    float ly = fmap(JS_L.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0);
    JSL.setJsPos(lx, ly);
    float rx = fmap(JS_R.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0);
    float ry = fmap(JS_R.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0);
    JSR.setJsPos(rx, ry);
}

void Task_KnobLimit()
{
    float kl = fmap(CTRL.KnobLimit.L, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    KnobLimitL.setScroll(kl);
    float kr = fmap(CTRL.KnobLimit.R, 0, CtrlOutput_MaxValue, 0.0, 1.0);
    KnobLimitR.setScroll(kr);
}


void Task_MPUdiaplay()
{
	float Mx = fmap(MPU_Data.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0);
	float My = fmap(MPU_Data.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -1.0, 1.0);
	MPU.setJsPos(Mx,My);
}

void Task_PrintSpeed()
{
    if(PassBack_State)
    {
        tft.setCursor(0, 0);
        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
        tft.setTextSize(2);
        if(CarSS_Slave.Channel_1 <= 99.9)
        {
            tft.printf("%4.1f", CarSS_Slave.Channel_1);
        }
    }
}
	
static void Setup()
{
	tft.clear();
    JSL.Color_FM = ST77XX_YELLOW;
    JSL.Color_JS = ST77XX_BLUE;
    JSR.Color_FM = ST77XX_YELLOW;
    JSR.Color_JS = ST77XX_CYAN;
	MPU.Color_FM = ST77XX_YELLOW;
	MPU.Color_JS = ST77XX_GREEN;
	KnobLimitL.Color_FM=ST77XX_YELLOW;
	KnobLimitR.Color_FM=ST77XX_YELLOW;
	KnobLimitL.setScroll(0.0);
	KnobLimitR.setScroll(0.0);
    for(float i = 1.0; i > 0.0; i -= 0.1)
    {
        JSL.setPosition(i * -50, JSL.Y);
        JSR.setPosition(95 + i * 50, JSR.Y);
		MPU.setPosition(95+i*50,MPU.Y);
		KnobLimitL.setPosition(KnobLimitL.X,63+i * 50);
		KnobLimitR.setPosition(KnobLimitR.X,63+i * 50);
    }
    JSL.setPosition(2, JSL.Y);

    mtmCtrlInfo.TaskRegister(0, Task_UpdateJsPos, 10);
    mtmCtrlInfo.TaskRegister(1, Task_KnobLimit, 20);
    mtmCtrlInfo.TaskRegister(2, Task_MPUdiaplay, 20);
	mtmCtrlInfo.TaskRegister(3, Task_PrintSpeed, 50);
}

static void Loop()
{
    mtmCtrlInfo.Running(millis());
}

static void Exit()
{
	for(float i = 0.0; i < 1.5; i += 0.1)
    {
        JSL.setPosition(i * -50, JSL.Y);
        JSR.setPosition(100 + i * 50, JSR.Y);
		MPU.setPosition(100+i*50,MPU.Y);
		KnobLimitL.setPosition(KnobLimitL.X,63+i * 50);
		KnobLimitR.setPosition(KnobLimitR.X,63+i * 50);
    }
}

static void ButtonPressEvent()
{
	if(btB)
	{
		pm.PageChangeTo(PAGE_MainMenu);
	}
}

static void ButtonLongPressEvent()
{
	
}

void PageRegister_CtrlInfo()
{
	#define ThisPage PAGE_CtrlInfo
	pm.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
	pm.PageRegister_ButtonPressEvent(ThisPage, ButtonPressEvent);
	pm.PageRegister_ButtonLongPressEvent(ThisPage, ButtonLongPressEvent);
}
