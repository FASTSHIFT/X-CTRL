#include "FileGroup.h"
#include "ComPrivate.h"
#include "DigitalFilter.h"

#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15
Joystick_t JS_L;
Joystick_t JS_R;


#define Channel_JS_LY (ADC_MaxValue - analogRead_DMA(Joystick_Y_DMA_CH))
#define Channel_JS_LX (ADC_MaxValue - analogRead_DMA(Joystick_X_DMA_CH))

static int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    if ((ADC_Value - MP) >= 0)
        return map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    else
        return map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
}

static void Read_Joystick()
{
    CTRL.Left.Y = JS_L.Y = JoystickMap(Channel_JS_LY, JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    CTRL.Left.X = JS_L.X = JoystickMap(Channel_JS_LX, JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
}

float BattVoltage,BattUsage;
static DigitalFilter<float> dfBattUsage(13);
static float Read_BattVoltage()
{
    BattVoltage = (analogRead_DMA(BattSensor_DMA_CH) / float(ADC_MaxValue)) * 3.3 * 2.0;
    __LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

static void Read_BattUsage()
{
    BattUsage = dfBattUsage.getSmooth(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0, 100.0));
}

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEventMonitor();
    Read_BattUsage();
}
