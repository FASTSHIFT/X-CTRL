#include "FileGroup.h"
#include "GUI_Private.h"
#include "DigitalFilter.h"

static MillisTaskManager mtm_Calibration(3);

static FilterAverage<int> dfJSL_Y_ADC(13);

void Task_JoystickCab()
{
    int16_t ADC_LY = dfJSL_Y_ADC.getNext(JSL_Y_ADC);

    if(ADC_LY > JS_L.Ymax)JS_L.Ymax = ADC_LY;
    if(ADC_LY < JS_L.Ymin)JS_L.Ymin = ADC_LY;
}

void Task_ShowJoystickStatus()
{
    screen.setCursor(StatusBarPos + 5, 14);
    screen.printf("Max:%4d",JS_L.Ymax);
    screen.setCursor(StatusBarPos + 5, 22);
    screen.printf("Min:%4d",JS_L.Ymin); 
}

static void Setup()
{
    screen.setCursor(StatusBarPos + 5, 5);
    screen.print("Calibrating...");
    BuzzTone(1000, 200);
    
    uint32_t LY_sum = 0.0;
    uint32_t cnt = 0;
    for(cnt = 0; cnt < 1000; cnt++)
    {
        LY_sum += dfJSL_Y_ADC.getNext(JSL_Y_ADC);
        delay(1);
    }

    JS_L.Ymid = LY_sum / (cnt + 1);

    JS_L.Ymax = ADC_MaxValue / 2 + 200, JS_L.Ymin = ADC_MaxValue / 2 - 200;

    mtm_Calibration.TaskRegister(0, Task_JoystickCab, 10);
    mtm_Calibration.TaskRegister(1, Task_ShowJoystickStatus, 50);
}

static void Loop()
{
    mtm_Calibration.Running(millis());
}

static void Exit()
{
    EEPROM_Handle(EEPROM_Chs::SaveData);
    ClearPage();
    BuzzTone(500, 200);
}

static void ButtonLongPressEvent()
{
    page.PageChangeTo(PAGE_CtrlInfo);
}

void PageRegister_Calibration(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
//    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
