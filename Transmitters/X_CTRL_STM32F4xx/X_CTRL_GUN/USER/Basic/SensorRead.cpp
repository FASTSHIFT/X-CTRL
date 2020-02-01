#include "FileGroup.h"
#include "DigitalFilter.h"
#include "ComPrivate.h"
#include "LuaGroup.h"
#include "math.h"

/*左摇杆*/
RCX::Joystick_t JS_L;

/*右摇杆*/
RCX::Joystick_t JS_R;

/*曲线设置*/
double JS_L_SlopeStart = 5.0f;
double JS_L_SlopeEnd  = 5.0f;

double JS_R_SlopeStart = 5.0f;
double JS_R_SlopeEnd  = 5.0f;

float CPU_Temperature = 0.0f;

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
  * @param  &js: 摇杆对象引用
  * @param  adc_x: X轴ADC值
  * @param  adc_y: Y轴ADC值
  * @retval 无
  */
static void JoystickMap(RCX::Joystick_t &js, int16_t adc_x, int16_t adc_y, double slopeStart, double slopeEnd)
{
    __LimitValue(adc_x, js.Xmin, js.Xmax);
    __LimitValue(adc_y, js.Ymin, js.Ymax);

    js.X = constrain(
               ((adc_x - js.Xmid) >= 0) ?
               NonlinearMap(adc_x, js.Xmid, js.Xmax, 0, RCX_ChannelData_Max, slopeStart, slopeEnd) :
               NonlinearMap(adc_x, js.Xmid, js.Xmin, 0, -RCX_ChannelData_Max, slopeStart, slopeEnd),
               -RCX_ChannelData_Max,
               RCX_ChannelData_Max
           );
    js.Y = constrain(
               ((adc_y - js.Ymid) >= 0) ?
               NonlinearMap(adc_y, js.Ymid, js.Ymax, 0, RCX_ChannelData_Max, slopeStart, slopeEnd) :
               NonlinearMap(adc_y, js.Ymid, js.Ymin, 0, -RCX_ChannelData_Max, slopeStart, slopeEnd),
               -RCX_ChannelData_Max,
               RCX_ChannelData_Max
           );
}

/**
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
static void Read_Joystick()
{
    JoystickMap(JS_L, JSL_X_ADC(), JSL_Y_ADC(), JS_L_SlopeStart, JS_L_SlopeEnd);
    JoystickMap(JS_R, JSR_X_ADC(), JSR_Y_ADC(), JS_R_SlopeStart, JS_R_SlopeEnd);
    
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
    
    CTRL.Left.X = JS_L.X;
    CTRL.Left.Y = JS_L.Y;
    CTRL.Right.X = JS_R.X;
    CTRL.Right.Y = JS_R.Y;
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
    CPU_Temperature = (dfCPUTemp.getNext(Temp_ADC()) * 3300.0f / 4095.0f - 760.0f) / 2.5f + 25;
}

/*长时间无操作监控使能*/
bool State_NoOperationMonitor = true;

/*无操作时间超时*/
const uint32_t NoOperationAlarmTimeoutMs = NoOperationTimeout * 1000;

/**
  * @brief  长时间无操作监控
  * @param  无
  * @retval 无
  */
static void NoOperationMonitor()
{
    if(!State_NoOperationMonitor)
        return;
    
    static uint32_t lastOperateTime = 0;
    
    /*摇杆是否被操作*/
    if(ABS(JS_L.X) + ABS(JS_L.Y) + ABS(JS_R.X) + ABS(JS_R.Y) > 400)
    {
        lastOperateTime = millis();
    }
    
    /*按键是否被操作*/
    if(btUP || btDOWN || btOK || btBACK)
    {
        lastOperateTime = millis();
    }
    
    if(millis() - lastOperateTime > NoOperationAlarmTimeoutMs)
    {
        BuzzMusic(MC_Type::MC_NoOperationWarning);
    }
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
    __IntervalExecute(NoOperationMonitor(), 5000);//长时间无操作监控
}
