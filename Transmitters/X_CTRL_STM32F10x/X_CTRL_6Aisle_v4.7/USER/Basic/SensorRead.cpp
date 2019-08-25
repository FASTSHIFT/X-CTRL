#include "FileGroup.h"
#include "DigitalFilter.h"
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
    return (
        (ADC_Value - mid) >= 0) ? 
       __Map(ADC_Value, mid, max, 0, CtrlOutput_MaxValue) : 
       __Map(ADC_Value, min, mid, -CtrlOutput_MaxValue, 0
    );
}

/**
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
static void Read_Joystick()
{
    CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);   //左摇杆Y轴映射
    CTRL.Left.Y = JS_L.Y = JoystickMap(JSL_Y_ADC(), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);   //左摇杆X轴映射
    CTRL.Right.X = JS_R.X = JoystickMap(JSR_X_ADC(), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);  //右摇杆Y轴映射
    CTRL.Right.Y = JS_R.Y = JoystickMap(JSR_Y_ADC(), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);  //右摇杆X轴映射

    /*读取二段开关*/
    if(digitalRead(SPDT_Switch_Pin))
    {
        CTRL.KnobLimit.L = CtrlOutput_MaxValue;
    }
    else
    {
        CTRL.KnobLimit.L = 0;
    }
    
    /*读取三段开关*/
    CTRL.KnobLimit.R = __Map(SPTT_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
}

/*电池电压，电池电量*/
float BattVoltage,BattUsage;
/*电池电压均值滤波器*/
static DigitalFilter<float> dfBattUsage(13);

/**
  * @brief  电池电压读取
  * @param  无
  * @retval 电池电压
  */
static float Read_BattVoltage()
{
    BattVoltage = (BattSensor_ADC() / float(ADC_MaxValue)) * 3.3 * 2.0;
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
    BattUsage = dfBattUsage.getSmooth(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0, 100.0));
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
    //EncoderMonitor();       //旋转编码器事件监视
    Read_BattUsage();       //更新电池状态
}
