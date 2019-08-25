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
    //static FilterAverage<float> dfJSL_Y_ADC(9);
    static PT1Filter dfJSL_Y_ADC(0.01, 10);
    
    float jsl_adc = JSL_Y_ADC;
    __LimitValue(jsl_adc, JS_L.Ymin, JS_L.Ymax);
    JS_L.Y = JoystickMap(dfJSL_Y_ADC.Next(jsl_adc), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    //CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC, JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
}

#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.20

/*电池电压，电池电量*/
float BattVoltage, BattUsage;

/*电池电压均值滤波器*/
static FilterAverage<float> dfBattVoltage(13);

/*电池电量滞回滤波器*/
static FilterHysteresis<float> dfBattUsage(3.0f);

/**
  * @brief  电池电压读取
  * @param  无
  * @retval 电池电压
  */
static float Read_BattVoltage()
{
    BattVoltage = dfBattVoltage.getNext((BattSensor_ADC / float(ADC_MaxValue)) * 3.3f * 2.0f);
    __LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

/**
  * @brief  电池电量读取
  * @param  无
  * @retval 无
  */
static void Read_BattUsage()
{
    /*从滤波器取出滤波后的电压值然后映射为0~100%的电量*/
    BattUsage = dfBattUsage.getNext(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0f, 100.0f));
}

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEventMonitor();
    Read_BattUsage();
}
