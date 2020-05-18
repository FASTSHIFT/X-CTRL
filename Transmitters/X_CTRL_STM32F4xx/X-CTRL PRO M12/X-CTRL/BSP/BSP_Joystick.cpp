#include "Basic/FileGroup.h"
#include "Communication/ComPrivate.h"
#include "Filters/Filters.h"
#include "math.h"
#include "JoystickMap/JoystickMap.h"

/*摇杆曲线表*/
#define JS_CURVE_POINT_SIZE 101
int16_t JoystickLeft_CurveMap[JS_CURVE_POINT_SIZE];
int16_t JoystickRight_CurveMap[JS_CURVE_POINT_SIZE];

/*摇杆映射管理器*/
static JoystickMap JoystickMap_LX(JoystickLeft_CurveMap, JS_CURVE_POINT_SIZE);
static JoystickMap JoystickMap_LY(JoystickLeft_CurveMap, JS_CURVE_POINT_SIZE);
static JoystickMap JoystickMap_RX(JoystickRight_CurveMap, JS_CURVE_POINT_SIZE);
static JoystickMap JoystickMap_RY(JoystickRight_CurveMap, JS_CURVE_POINT_SIZE);

/*摇杆一阶低通滤波器*/
static PT1Filter Filter_LX(0.01f, 10);
static PT1Filter Filter_LY(0.01f, 10);
static PT1Filter Filter_RX(0.01f, 10);
static PT1Filter Filter_RY(0.01f, 10);

static void Joystick_UpdateConfig(JoystickMap* jm, Channel_TypeDef* config)
{
    jm->SetInputReference(
        config->Min,
        config->Mid,
        config->Max
    );
    jm->SetOutputMax(RCX_CHANNEL_DATA_MAX);
    jm->SetCurve(config->Curve.Start, config->Curve.End);
}

static void Joystick_StructInit(Channel_TypeDef* config)
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
    uint16_t adc_lx = JSL_X_ADC();
    uint16_t adc_ly = JSL_Y_ADC();
    uint16_t adc_rx = JSR_X_ADC();
    uint16_t adc_ry = JSR_Y_ADC();
    
    /*如果使能滤波器*/
    if(CTRL.State.JostickFilter)
    {
        adc_lx = Filter_LX.Next(adc_lx);
        adc_ly = Filter_LY.Next(adc_ly);
        adc_rx = Filter_RX.Next(adc_rx);
        adc_ry = Filter_RY.Next(adc_ry);
    }
    
    CTRL.JS_L.X.Val = JoystickMap_LX.GetNext(adc_lx);
    CTRL.JS_L.Y.Val = JoystickMap_LY.GetNext(adc_ly);
    CTRL.JS_R.X.Val = JoystickMap_RX.GetNext(adc_rx);
    CTRL.JS_R.Y.Val = JoystickMap_RY.GetNext(adc_ry);
}
