#include "FileGroup.h"
#include "ComPrivate.h"
#include "DigitalFilter.h"

Joystick_t JS_L;
Joystick_t JS_R;

static int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    __LimitValue(ADC_Value, MIN, MAX);
    if ((ADC_Value - MP) >= 0)
        return map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    else
        return map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
}

static void Read_Joystick()
{
    int16_t JSL_Y_Adc = JSL_Y_ADC();
    __LimitValue(JSL_Y_Adc, JS_L.Ymin, JS_L.Ymax);
    CTRL.Left.Y = JS_L.Y = map(JSL_Y_Adc, JS_L.Ymin, JS_L.Ymax, 0, CtrlOutput_MaxValue);
    CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    
    CTRL.Right.Y = JS_R.Y = JoystickMap(JSR_Y_ADC(), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);  
    CTRL.Right.X = JS_R.X = JoystickMap(JSR_X_ADC(), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);

    if(!digitalRead(SPDT_Switch_Pin))
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
    BattVoltage = dfBattVoltage.getNext((BattSensor_ADC() / float(ADC_MaxValue)) * 3.3f * 2.0f);
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
	Read_BattUsage();
    ButtonEventMonitor();
	EncoderMonitor();
}
