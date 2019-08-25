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
        map(ADC_Value, mid, max, 0, CtrlOutput_MaxValue) : 
        map(ADC_Value, min, mid, -CtrlOutput_MaxValue, 0
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

    if(State_MPU)//是否启动重力感应
    {
        /*将MPU所产生的数据替代摇杆值*/
        extern int16_t *from_MPU_LX, *from_MPU_LY, *from_MPU_RX, *from_MPU_RY;
        if(from_MPU_LX) CTRL.Left.X = *from_MPU_LX;
        if(from_MPU_LY) CTRL.Left.Y = *from_MPU_LY;
        if(from_MPU_RX) CTRL.Right.X = *from_MPU_RX;
        if(from_MPU_RY) CTRL.Right.Y = *from_MPU_RY;
    }

    if(digitalRead(SPDT_Switch_Pin))//是否开启解锁开关
    {
        CTRL.KnobLimit.L = map(ADL_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//左限幅旋钮读取
        CTRL.KnobLimit.R = map(ADR_ADC(), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);//右限幅旋钮读取
    }
    else
    {
        /*清零限幅*/
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
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
    EncoderMonitor();       //旋转编码器事件监视
    Read_BattUsage();       //更新电池状态
}
