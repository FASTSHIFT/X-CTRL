#include "FileGroup.h"
#include "DigitalFilter.h"

extern void ButtonEventMonitor();

Joystick_t JS_L;

static int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    if ((ADC_Value - MP) >= 0)
        return map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    else
        return map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
}

static void Read_Joystick()
{
    static DigitalFilter<int> dfJSL_Y_ADC(13);

    int16_t jsl_adc = JSL_Y_ADC;
    __LimitValue(jsl_adc, JS_L.Ymin, JS_L.Ymax);
    JS_L.Y = JoystickMap(dfJSL_Y_ADC.getSmooth(jsl_adc), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    //CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC, JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
}

#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.20
float BattVoltage, BattUsage;

static float Read_BattVoltage()
{
    BattVoltage = (BattSensor_ADC / float(ADC_MaxValue)) * 3.3 * 2.0;
    __LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

static void Read_BattUsage()
{
    static DigitalFilter<float> dfBattUsage(13);
    BattUsage = dfBattUsage.getSmooth(
                    __Map(
                        Read_BattVoltage(),
                        BattEmptyVoltage,
                        BattFullVoltage,
                        0.0,
                        100.0
                    )
                );
}

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEventMonitor();
    Read_BattUsage();
}
