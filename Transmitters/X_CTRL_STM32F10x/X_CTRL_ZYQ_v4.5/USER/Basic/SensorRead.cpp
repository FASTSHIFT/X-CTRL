#include "FileGroup.h"
#include "DigitalFilter.h"

Joystick_t JS_L;
Joystick_t JS_R;

static int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    if ((ADC_Value - MP) >= 0)
        return map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    else
        return map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
}

static void Read_Joystick()
{
    extern int16_t *from_MPU_LX, *from_MPU_LY, *from_MPU_RX, *from_MPU_RY;
    CTRL.Left.Y = JS_L.Y = JoystickMap(JSL_Y_ADC(), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    
    CTRL.Right.Y = JS_R.Y = JoystickMap(JSR_Y_ADC(), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);  
    CTRL.Right.X = JS_R.X = JoystickMap(JSR_X_ADC(), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);

    if(State_MPU)
    {
        if(from_MPU_LX) CTRL.Left.X = *from_MPU_LX;
        if(from_MPU_LY) CTRL.Left.Y = *from_MPU_LY;
        if(from_MPU_RX) CTRL.Right.X = *from_MPU_RX;
        if(from_MPU_RY) CTRL.Right.Y = *from_MPU_RY;
    }

    if(true/*digitalRead(SPDT_Switch_Pin)*/)
    {
        CTRL.KnobLimit.L = map(ADL_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
        CTRL.KnobLimit.R = map(ADR_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    }
    else
    {
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
}

float BattVoltage = 4.2;
float BattUsage = 100.0;

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEventMonitor();
}
