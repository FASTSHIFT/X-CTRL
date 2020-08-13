#include "Basic/FileGroup.h"
#include "Communication/ComPrivate.h"
#include "Filters/Filters.h"
#include "math.h"
#include "BSP/BSP.h"
#include "JoystickMap/JoystickMap.h"

/*摇杆曲线表*/
#define JS_CURVE_POINT_SIZE 101
static int16_t JoystickLeft_CurveMap[JS_CURVE_POINT_SIZE];
static int16_t JoystickRight_CurveMap[JS_CURVE_POINT_SIZE];

typedef struct{
    XC_Channel_TypeDef* pJS;  /*摇杆参数地址*/
    JoystickMap         Map;  /*摇杆映射管理器*/
    PT1Filter           Flt;  /*摇杆一阶低通滤波器*/
}JoystickGrp_TypeDef;

typedef enum{
    IDX_LX,
    IDX_LY,
    IDX_RX,
    IDX_RY,
    IDX_MAX
}JoystickIndex_Type;

static JoystickGrp_TypeDef Joystick_Grp[IDX_MAX]={
    {&CTRL.JS_L.X, JoystickMap(JoystickLeft_CurveMap,  JS_CURVE_POINT_SIZE), PT1Filter(0.01f, 10)},
    {&CTRL.JS_L.Y, JoystickMap(JoystickLeft_CurveMap,  JS_CURVE_POINT_SIZE), PT1Filter(0.01f, 10)},
    {&CTRL.JS_R.X, JoystickMap(JoystickRight_CurveMap, JS_CURVE_POINT_SIZE), PT1Filter(0.01f, 10)},
    {&CTRL.JS_R.Y, JoystickMap(JoystickRight_CurveMap, JS_CURVE_POINT_SIZE), PT1Filter(0.01f, 10)},
};

static void Joystick_UpdateConfig(JoystickMap* jm, XC_Channel_TypeDef* pJS)
{
    jm->SetInputReference(
        pJS->Min,
        pJS->Mid,
        pJS->Max
    );
    jm->SetOutputMax(RCX_CHANNEL_DATA_MAX);
    jm->SetCurve(pJS->Curve.Start, pJS->Curve.End);
}

static void Joystick_StructInit(XC_Channel_TypeDef* pJS)
{
    pJS->Min = 0;
    pJS->Mid = JS_ADC_MAX / 2;
    pJS->Max = JS_ADC_MAX;
    pJS->Curve.Start = 5.0f;
    pJS->Curve.End = 5.0f;
}

void Joystick_SetDefault()
{
    for(int i = 0; i < __Sizeof(Joystick_Grp); i++)
    {
        Joystick_StructInit(Joystick_Grp[i].pJS);
        Joystick_UpdateConfig(&Joystick_Grp[i].Map, Joystick_Grp[i].pJS);
    }
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
        Joystick_Grp[IDX_LX].Map.SetCurve(startK, endK);
        Joystick_Grp[IDX_LY].Map.SetCurve(startK, endK);
    }
    else if(js == &CTRL.JS_R)
    {
        Joystick_Grp[IDX_RX].Map.SetCurve(startK, endK);
        Joystick_Grp[IDX_RY].Map.SetCurve(startK, endK);
    }
}

void Joystick_GetCurve(XC_Joystick_TypeDef* js, int16_t* points, uint32_t size)
{
    uint16_t min = js->X.Min;
    uint16_t max = js->X.Max;
    
    JoystickMap* jm = (js == &CTRL.JS_L) ? &Joystick_Grp[IDX_LX].Map : &Joystick_Grp[IDX_RX].Map;
    
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
  * @brief  摇杆读取
  * @param  无
  * @retval 无
  */
void Joystick_Update()
{
    uint16_t adcVal[IDX_MAX];
    adcVal[IDX_LX] = JSL_X_ADC();
    adcVal[IDX_LY] = JSL_Y_ADC();
    adcVal[IDX_RX] = JSR_X_ADC();
    adcVal[IDX_RY] = JSR_Y_ADC();
    
    for(int i = 0; i < __Sizeof(Joystick_Grp); i++)
    {
        /*如果使能滤波器*/
        if(CTRL.State->JostickFilter)
        {
            adcVal[i] = Joystick_Grp[i].Flt.Next(adcVal[i]);
        }
        
        Joystick_Grp[i].pJS->AdcVal = adcVal[i];
        Joystick_Grp[i].pJS->Val = Joystick_Grp[i].Map.GetNext(adcVal[i]);
    }
}
