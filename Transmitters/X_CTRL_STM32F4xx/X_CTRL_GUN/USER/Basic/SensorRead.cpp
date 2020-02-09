#include "FileGroup.h"
#include "DigitalFilter.h"
#include "ComPrivate.h"
#include "LuaGroup.h"
#include "math.h"

/*控制器*/
CTRL_TypeDef CTRL;

static void JoystickInit()
{
    CTRL.JS_L.X.Min = 0;
    CTRL.JS_L.X.Mid = ADC_MaxValue / 2;
    CTRL.JS_L.X.Max = ADC_MaxValue;
    CTRL.JS_L.X.Curve.Start = CTRL.JS_L.X.Curve.End = 5.0f;

    CTRL.JS_L.Y.Min = 1703;
    CTRL.JS_L.Y.Mid = 2020;
    CTRL.JS_L.Y.Max = 2929;
    CTRL.JS_L.Y.Curve.Start = CTRL.JS_L.Y.Curve.End = 5.0f;

    CTRL.JS_R.X.Min = 324;
    CTRL.JS_R.X.Mid = 2017;
    CTRL.JS_R.X.Max = 3917;
    CTRL.JS_R.X.Curve.Start = CTRL.JS_R.X.Curve.End = 5.0f;

    CTRL.JS_R.Y.Min = 0;
    CTRL.JS_R.Y.Mid = ADC_MaxValue / 2;
    CTRL.JS_R.Y.Max = ADC_MaxValue;
    CTRL.JS_R.Y.Curve.Start = CTRL.JS_R.Y.Curve.End = 5.0f;
}

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
  * @param  *js: 摇杆对象地址
  * @param  adc_x: X轴ADC值
  * @param  adc_y: Y轴ADC值
  * @retval 无
  */
static void JoystickMap(Joystick_TypeDef* js, int16_t adc_x, int16_t adc_y)
{
    __LimitValue(adc_x, js->X.Min, js->X.Max);
    __LimitValue(adc_y, js->Y.Min, js->Y.Max);

    js->X.Val = constrain(
                    ((adc_x - js->X.Mid) >= 0) ?
                    NonlinearMap(adc_x, js->X.Mid, js->X.Max, 0, RCX_ChannelData_Max, js->X.Curve.Start, js->X.Curve.End) :
                    NonlinearMap(adc_x, js->X.Mid, js->X.Min, 0, -RCX_ChannelData_Max, js->X.Curve.Start, js->X.Curve.End),
                    -RCX_ChannelData_Max,
                    RCX_ChannelData_Max
                );
    js->Y.Val = constrain(
                    ((adc_y - js->Y.Mid) >= 0) ?
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Max, 0, RCX_ChannelData_Max, js->Y.Curve.Start, js->Y.Curve.End) :
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Min, 0, -RCX_ChannelData_Max, js->Y.Curve.Start, js->Y.Curve.End),
                    -RCX_ChannelData_Max,
                    RCX_ChannelData_Max
                );
}

/**
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
static void JoystickUpdate()
{
    JoystickMap(&CTRL.JS_L, JSL_X_ADC(), JSL_Y_ADC());
    JoystickMap(&CTRL.JS_R, JSR_X_ADC(), JSR_Y_ADC());

    if(IS_EnableCtrl())//是否开启解锁开关
    {
        CTRL.KnobLimit.L = DR_TH_Value;//map(ADL_ADC(), 0, ADC_MaxValue, 0, RCX_ChannelData_Max);//左限幅旋钮读取
        CTRL.KnobLimit.R = DR_ST_Value;//map(ADR_ADC(), 0, ADC_MaxValue, 0, RCX_ChannelData_Max);//右限幅旋钮读取
    }
    else
    {
        /*限幅置0*/
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
}

/*电池电压均值滤波器*/
static FilterAverage<float> dfBattVoltage(13);

/*电池电量滞回滤波器*/
static FilterHysteresis<float> dfBattUsage(3.0f);

/**
  * @brief  电池电压读取
  * @param  无
  * @retval 电池电压
  */
static float BattVoltageUpdate()
{
    CTRL.Battery.Voltage = dfBattVoltage.getNext((BattSensor_ADC() / float(ADC_MaxValue)) * 3.3f * 2.0f);
    __LimitValue(CTRL.Battery.Voltage, BattEmptyVoltage, BattFullVoltage);
    return (CTRL.Battery.Voltage);
}

/**
  * @brief  电池电量读取
  * @param  无
  * @retval 无
  */
static void BattUsageUpdate()
{
    /*从滤波器取出滤波后的电压值然后映射为0~100%的电量*/
    CTRL.Battery.Usage = dfBattUsage.getNext(__Map(BattVoltageUpdate(), BattEmptyVoltage, BattFullVoltage, 0.0f, 100.0f));
}

/*无操作时间超时*/
const uint32_t IdleWarnTimeoutMs = IdleWarnTimeout * 1000;

/**
  * @brief  长时间无操作监控
  * @param  无
  * @retval 无
  */
static void IdleWarnMonitor()
{
    if(!CTRL.State.IdleWarn)
        return;

    static uint32_t lastOperateTime = 0;

    /*摇杆是否被操作*/
    if(ABS(CTRL.JS_L.X.Val) + ABS(CTRL.JS_L.Y.Val) + ABS(CTRL.JS_R.X.Val) + ABS(CTRL.JS_R.Y.Val) > 400)
    {
        lastOperateTime = millis();
    }

    /*按键是否被操作*/
    if(btUP || btDOWN || btOK || btBACK)
    {
        lastOperateTime = millis();
    }

    if(millis() - lastOperateTime > IdleWarnTimeoutMs)
    {
        BuzzMusic(MC_Type::MC_NoOperationWarning);
    }
}

/**
  * @brief  传感器初始化
  * @param  无
  * @retval 无
  */
void Init_Sensors()
{
    DEBUG_FUNC();
    pinMode(BattCharge_Pin, INPUT_PULLUP);

    /*ADC引脚初始化*/
    pinMode(JSL_Y_Pin, INPUT_ANALOG_DMA);
    pinMode(JSR_X_Pin, INPUT_ANALOG_DMA);
    pinMode(BattSensor_Pin, INPUT_ANALOG_DMA);
    ADC_DMA_Register(ADC_Channel_TempSensor);
    ADC_DMA_Init();

    Init_BottonEvent();     //初始化按键事件
    Init_EncoderEvent();    //初始化旋转编码器事件
    JoystickInit();
}

/**
  * @brief  电池电量读取
  * @param  无
  * @retval 无
  */
void Task_SensorUpdate()
{
    JoystickUpdate();       //读取摇杆
    ButtonEventMonitor();   //按键事件监视
#ifdef USE_Encoder
    EncoderMonitor();       //旋转编码器事件监视
#endif
    BattUsageUpdate();       //更新电池状态
    __IntervalExecute(IdleWarnMonitor(), 5000);//长时间无操作监控
}
