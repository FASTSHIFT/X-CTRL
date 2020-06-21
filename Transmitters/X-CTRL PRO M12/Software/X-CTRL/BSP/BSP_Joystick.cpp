#include "Basic/FileGroup.h"
#include "Communication/ComPrivate.h"
#include "Filters/Filters.h"
#include "math.h"
#include "BSP/BSP.h"
#include "JoystickMap/JoystickMap.h"

/*Ò¡¸ËÇúÏß±í*/
#define JS_CURVE_POINT_SIZE 101
static int16_t JoystickLeft_CurveMap[JS_CURVE_POINT_SIZE];
static int16_t JoystickRight_CurveMap[JS_CURVE_POINT_SIZE];

/*Ò¡¸ËÓ³Éä¹ÜÀíÆ÷*/
static JoystickMap JoystickMap_LX(JoystickLeft_CurveMap, JS_CURVE_POINT_SIZE);
static JoystickMap JoystickMap_LY(JoystickLeft_CurveMap, JS_CURVE_POINT_SIZE);
static JoystickMap JoystickMap_RX(JoystickRight_CurveMap, JS_CURVE_POINT_SIZE);
static JoystickMap JoystickMap_RY(JoystickRight_CurveMap, JS_CURVE_POINT_SIZE);

/*Ò¡¸ËÒ»½×µÍÍ¨ÂË²¨Æ÷*/
static PT1Filter Filter_LX(0.01f, 10);
static PT1Filter Filter_LY(0.01f, 10);
static PT1Filter Filter_RX(0.01f, 10);
static PT1Filter Filter_RY(0.01f, 10);

static void Joystick_UpdateConfig(JoystickMap* jm, XC_Channel_TypeDef* config)
{
    jm->SetInputReference(
        config->Min,
        config->Mid,
        config->Max
    );
    jm->SetOutputMax(RCX_CHANNEL_DATA_MAX);
    jm->SetCurve(config->Curve.Start, config->Curve.End);
}

static void Joystick_StructInit(XC_Channel_TypeDef* config)
{
    config->Min = 0;
    config->Mid = JS_ADC_MAX / 2;
    config->Max = JS_ADC_MAX;
    config->Curve.Start = 5.0f;
    config->Curve.End = 5.0f;
}

void Joystick_SetDefault()
{
    Joystick_StructInit(&CTRL.JS_L.X);
    Joystick_UpdateConfig(&JoystickMap_LX, &CTRL.JS_L.X);
    
    Joystick_StructInit(&CTRL.JS_L.Y);
    Joystick_UpdateConfig(&JoystickMap_LY, &CTRL.JS_L.Y);
    
    Joystick_StructInit(&CTRL.JS_R.X);
    Joystick_UpdateConfig(&JoystickMap_RX, &CTRL.JS_R.X);
    
    Joystick_StructInit(&CTRL.JS_R.Y);
    Joystick_UpdateConfig(&JoystickMap_RY, &CTRL.JS_R.Y);
}

void Joystick_SetCurve(XC_Joystick_TypeDef* js, float startK, float endK)
{
    __LimitValue(startK, 0.1f, 9.9f);
    __LimitValue(endK, 0.1f, 9.9f);
    js->X.Curve.Start = startK;
    js->X.Curve.End = endK;
    js->Y.Curve.Start = startK;
    js->Y.Curve.End = endK;
    if(js == &CTRL.JS_L)
    {
        JoystickMap_LX.SetCurve(startK, endK);
        JoystickMap_LY.SetCurve(startK, endK);
    }
    else if(js == &CTRL.JS_R)
    {
        JoystickMap_RX.SetCurve(startK, endK);
        JoystickMap_RY.SetCurve(startK, endK);
    }
}

void Joystick_GetCurve(XC_Joystick_TypeDef* js, int16_t* points, uint32_t size)
{
    uint16_t min = js->X.Min;
    uint16_t max = js->X.Max;
    
    JoystickMap* jm = (js == &CTRL.JS_L) ? &JoystickMap_LX : &JoystickMap_RX;;
    
    for(int i = 0; i < size; i++)
    {
        int16_t virtual_adc = map(i, 0, size - 1, min, max);
        points[i] = jm->GetNext(virtual_adc);
    }
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
  * @brief  Ò¡¸Ë¶ÁÈ¡
  * @param  ÎÞ
  * @retval ÎÞ
  */
void Joystick_Update()
{
    uint16_t adc_lx = JSL_X_ADC();
    uint16_t adc_ly = JSL_Y_ADC();
    uint16_t adc_rx = JSR_X_ADC();
    uint16_t adc_ry = JSR_Y_ADC();
    
    /*Èç¹ûÊ¹ÄÜÂË²¨Æ÷*/
    if(CTRL.State->JostickFilter)
    {
        adc_lx = Filter_LX.Next(adc_lx);
        adc_ly = Filter_LY.Next(adc_ly);
        adc_rx = Filter_RX.Next(adc_rx);
        adc_ry = Filter_RY.Next(adc_ry);
    }
    
    CTRL.JS_L.X.AdcVal = adc_lx;
    CTRL.JS_L.Y.AdcVal = adc_ly;
    CTRL.JS_R.X.AdcVal = adc_rx;
    CTRL.JS_R.Y.AdcVal = adc_ry;
    
    CTRL.JS_L.X.Val = JoystickMap_LX.GetNext(adc_lx);
    CTRL.JS_L.Y.Val = JoystickMap_LY.GetNext(adc_ly);
    CTRL.JS_R.X.Val = JoystickMap_RX.GetNext(adc_rx);
    CTRL.JS_R.Y.Val = JoystickMap_RY.GetNext(adc_ry);
}
