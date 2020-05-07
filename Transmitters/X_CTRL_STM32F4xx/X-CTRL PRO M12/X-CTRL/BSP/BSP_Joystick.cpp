#include "Basic/FileGroup.h"
#include "Communication/ComPrivate.h"
#include "math.h"

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
float NonlinearMap(float value, float in_min, float in_max, float out_min, float out_max, float startK, float endK)
{
    if(ABS(startK - endK) < 0.001f)
        return __Map(value, in_min, in_max, out_min, out_max);

    float stY = exp(startK);
    float edY = exp(endK);
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
static void Joystick_Map(Joystick_TypeDef* js, int16_t adc_x, int16_t adc_y)
{
    __LimitValue(adc_x, js->X.Min, js->X.Max);
    __LimitValue(adc_y, js->Y.Min, js->Y.Max);

    int16_t x_val = ((adc_x - js->X.Mid) >= 0) ?
                    NonlinearMap(adc_x, js->X.Mid, js->X.Max, 0, RCX_CHANNEL_DATA_MAX, js->X.Curve.Start, js->X.Curve.End) :
                    NonlinearMap(adc_x, js->X.Mid, js->X.Min, 0, -RCX_CHANNEL_DATA_MAX, js->X.Curve.Start, js->X.Curve.End);
    js->X.Val = constrain(x_val, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX);
    
    int16_t y_val = ((adc_y - js->Y.Mid) >= 0) ?
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Max, 0, RCX_CHANNEL_DATA_MAX, js->Y.Curve.Start, js->Y.Curve.End) :
                    NonlinearMap(adc_y, js->Y.Mid, js->Y.Min, 0, -RCX_CHANNEL_DATA_MAX, js->Y.Curve.Start, js->Y.Curve.End);
    js->Y.Val = constrain(y_val, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX);
}

void Joystick_SetDefault()
{
    CTRL.JS_L.X.Min = 0;
    CTRL.JS_L.X.Mid = JS_ADC_MAX / 2;
    CTRL.JS_L.X.Max = JS_ADC_MAX;
    CTRL.JS_L.X.Curve.Start = CTRL.JS_L.X.Curve.End = 5.0f;

    CTRL.JS_L.Y.Min = 0;
    CTRL.JS_L.Y.Mid = JS_ADC_MAX / 2;
    CTRL.JS_L.Y.Max = JS_ADC_MAX;
    CTRL.JS_L.Y.Curve.Start = CTRL.JS_L.Y.Curve.End = 5.0f;

    CTRL.JS_R.X.Min = 0;
    CTRL.JS_R.X.Mid = JS_ADC_MAX / 2;
    CTRL.JS_R.X.Max = JS_ADC_MAX;
    CTRL.JS_R.X.Curve.Start = CTRL.JS_R.X.Curve.End = 5.0f;

    CTRL.JS_R.Y.Min = 0;
    CTRL.JS_R.Y.Mid = JS_ADC_MAX / 2;
    CTRL.JS_R.Y.Max = JS_ADC_MAX;
    CTRL.JS_R.Y.Curve.Start = CTRL.JS_R.Y.Curve.End = 5.0f;
}

void Joystick_Init()
{
    DEBUG_FUNC_LOG();
    
    pinMode(JSL_X_Pin, INPUT_ANALOG_DMA);
    pinMode(JSL_Y_Pin, INPUT_ANALOG_DMA);
    pinMode(JSR_X_Pin, INPUT_ANALOG_DMA);
    pinMode(JSR_Y_Pin, INPUT_ANALOG_DMA);
    ADC_DMA_Init();
}

/**
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
void Joystick_Update()
{
    Joystick_Map(&CTRL.JS_L, JSL_X_ADC(), JSL_Y_ADC());
    Joystick_Map(&CTRL.JS_R, JSR_X_ADC(), JSR_Y_ADC());
}
