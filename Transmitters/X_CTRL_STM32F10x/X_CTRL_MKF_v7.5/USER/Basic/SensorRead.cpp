#include "FileGroup.h"
#include "DigitalFilter.h"
#include "ComPrivate.h"
#include "math.h"

/*左摇杆*/
Joystick_t JS_L;

/*右摇杆*/
Joystick_t JS_R;

/*曲线设置*/
double JS_L_SlopeStart = 5.0f;
double JS_L_SlopeEnd  = 5.0f;

double JS_R_SlopeStart = 5.0f;
double JS_R_SlopeEnd  = 5.0f;

float CPU_Temperature = 0.0f;

/**
  * @brief  将一个值的变化区间非线性映射到另一个区间
  * @param  x:被映射的值
  * @param  in_min:被映射的值的最小值
  * @param  in_min:被映射的值的最大值
  * @param  out_min:被映射的值的最小值
  * @param  out_min:被映射的值的最大值
  * @param  startK:起点斜率
  * @param  endK:终点斜率
  * @retval 映射值输出
  */
double NonlinearMap(double value, double in_min, double in_max, double out_min, double out_max, double startK, double endK)
{
    if(ABS(startK - endK) < 0.00001f)
        return __Map(value, in_min, in_max, out_min, out_max);

    double stY = exp(startK);
    double edY = exp(endK);
    float x = __Map(value, in_min, in_max, startK, endK);

    return __Map(exp(x), stY, edY, out_min, out_max);
}

/**
  * @brief  摇杆值映射
  * @param  &js: 摇杆对象引用
  * @param  adc_x: X轴ADC值
  * @param  adc_y: Y轴ADC值
  * @retval 无
  */
static void JoystickMap(Joystick_t &js, int16_t adc_x, int16_t adc_y, double slopeStart, double slopeEnd)
{
    __LimitValue(adc_x, js.Xmin, js.Xmax);
    __LimitValue(adc_y, js.Ymin, js.Ymax);

    js.X = constrain(
               ((adc_x - js.Xmid) >= 0) ?
               NonlinearMap(adc_x, js.Xmid, js.Xmax, 0, CtrlOutput_MaxValue, slopeStart, slopeEnd) :
               NonlinearMap(adc_x, js.Xmid, js.Xmin, 0, -CtrlOutput_MaxValue, slopeStart, slopeEnd),
               -CtrlOutput_MaxValue,
               CtrlOutput_MaxValue
           );
    js.Y = constrain(
               ((adc_y - js.Ymid) >= 0) ?
               NonlinearMap(adc_y, js.Ymid, js.Ymax, 0, CtrlOutput_MaxValue, slopeStart, slopeEnd) :
               NonlinearMap(adc_y, js.Ymid, js.Ymin, 0, -CtrlOutput_MaxValue, slopeStart, slopeEnd),
               -CtrlOutput_MaxValue,
               CtrlOutput_MaxValue
           );
}

static int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    __LimitValue(ADC_Value, MIN, MAX);
    if ((ADC_Value - MP) >= 0)
        return map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    else
        return map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
}

/**
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
static void Read_Joystick()
{
//    JoystickMap(JS_L, JSL_X_ADC(), JSL_Y_ADC(), JS_L_SlopeStart, JS_L_SlopeEnd);
    int16_t JSL_Y_Adc = JSL_Y_ADC();
    __LimitValue(JSL_Y_Adc, JS_L.Ymin, JS_L.Ymax);
    CTRL.Left.Y = JS_L.Y = map(JSL_Y_Adc, JS_L.Ymin, JS_L.Ymax, 0, CtrlOutput_MaxValue);
    CTRL.Left.X = JS_L.X = JoystickMap(JSL_X_ADC(), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    
    JoystickMap(JS_R, JSR_X_ADC(), JSR_Y_ADC(), JS_R_SlopeStart, JS_R_SlopeEnd);
    
    CTRL.Left.X = JS_L.X;
    CTRL.Left.Y = JS_L.Y;
    CTRL.Right.X = JS_R.X;
    CTRL.Right.Y = JS_R.Y;

    if(digitalRead(SPDT_Switch_Pin))//是否开启解锁开关
    {
        CTRL.KnobLimit.L = map(ADL_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//左限幅旋钮读取
        CTRL.KnobLimit.R = map(ADR_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//右限幅旋钮读取
    }
    else
    {
        /*限幅置0*/
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
}

/*电池电压，电池电量*/
float BattVoltage, BattUsage;

/*电池电压均值滤波器*/
static FilterAverage<float> dfBattVoltage(13);

/*电池电量滞回滤波器*/
static FilterHysteresis<float> dfBattUsage(3.0f);

/*CPU温度均值滤波器*/
static FilterAverage<int> dfCPUTemp(13);

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

/**
  * @brief  CPU温度读取
  * @param  无
  * @retval 无
  */
static void Read_CPU_Temperature()
{
	CPU_Temperature = (dfCPUTemp.getNext(Temp_ADC()) * 3300.0f/4095.0f - 760.0f)/2.5f + 25;
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
#ifdef USE_Encoder
    EncoderMonitor();       //旋转编码器事件监视
#endif
    Read_BattUsage();       //更新电池状态
	Read_CPU_Temperature(); //更新CPU温度
}
