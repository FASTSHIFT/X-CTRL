#include "FileGroup.h"
//#include "DigitalFilter.h"
#include "Filters.h"
#include "ComPrivate.h"

/*左摇杆*/
Joystick_t JS_L;

/*右摇杆*/
Joystick_t JS_R;

/**
  * @brief  摇杆值映射
  * @param  ADC_Value: ADC值
  * @param  min: 摇杆ADC最小值
  * @param  min: 摇杆ADC中值
  * @param  min: 摇杆ADC最大值
  * @retval 无
  */
static int16_t JoystickMap(int16_t ADC_Value, int16_t min, int16_t mid, int16_t max)
{
    __LimitValue(ADC_Value, min, max);
    return (
           (ADC_Value - mid) >= 0) ?
           map(ADC_Value, mid, max, 0, CtrlOutput_MaxValue) :
           map(ADC_Value, min, mid, -CtrlOutput_MaxValue, 0
    );
}

volatile uint16_t jsL_x_adc = JSL_X_ADC();
volatile uint16_t jsL_y_adc = JSL_Y_ADC();
volatile uint16_t jsR_x_adc = JSR_X_ADC();
volatile uint16_t jsR_y_adc = JSR_Y_ADC();

PT1Filter Filter_JSL_X(0.005, 5);
PT1Filter Filter_JSL_Y(0.005, 5);
PT1Filter Filter_JSR_X(0.005, 5);
PT1Filter Filter_JSR_Y(0.005, 5);

/**
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
static void Read_Joystick()
{
		jsL_x_adc = Filter_JSL_X.Next(JSL_X_ADC());
		jsL_y_adc = Filter_JSL_Y.Next(JSL_Y_ADC());
		jsR_x_adc = Filter_JSR_X.Next(JSR_X_ADC());
		jsR_y_adc = Filter_JSR_Y.Next(JSR_Y_ADC());
	
    JS_L.X = constrain(JoystickMap(jsL_x_adc, JS_L.Xmin, JS_L.Xmid, JS_L.Xmax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);   //左摇杆X轴映射
    JS_L.Y = constrain(JoystickMap(jsL_y_adc, JS_L.Ymin, JS_L.Ymid, JS_L.Ymax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);   //左摇杆Y轴映射
    JS_R.X = constrain(JoystickMap(jsR_x_adc, JS_R.Xmin, JS_R.Xmid, JS_R.Xmax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);  //右摇杆X轴映射
    JS_R.Y = constrain(JoystickMap(jsR_y_adc, JS_R.Ymin, JS_R.Ymid, JS_R.Ymax), -CtrlOutput_MaxValue, CtrlOutput_MaxValue);  //右摇杆Y轴映射
}

/*电池电压，电池电量*/
float BattVoltage, BattUsage;
/*电池电压均值滤波器*/
//static DigitalFilter<float> dfBattUsage(13);
static PT1Filter dfBattUsage(0.005, 1);

/**
  * @brief  电池电压读取
  * @param  无
  * @retval 电池电压
  */
static float Read_BattVoltage()
{
    BattVoltage = (BattSensor_ADC() / float(ADC_MaxValue)) * 3.3f * 2.0f;
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
    BattUsage = dfBattUsage.Next(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0f, 100.0f));
}

/**
  * @brief  电池电量读取
  * @param  无
  * @retval 无
  */
void Task_SensorUpdate()
{
    Read_Joystick();        //读取摇杆
    ButtonEventMonitor();   //按键事件监视
    EncoderMonitor();       //旋转编码器事件监视
    Read_BattUsage();       //更新电池状态
}
