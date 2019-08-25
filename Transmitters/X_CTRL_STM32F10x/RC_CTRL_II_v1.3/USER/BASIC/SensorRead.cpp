#include "FileGroup.h"
#include "MPU_Private.h"

Joystick_t JS_L;
Joystick_t JS_R;

int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    int16_t JoystickValue;
    if ((ADC_Value - MP) >= 0)
    {
        JoystickValue = map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    }
    else
    {
        JoystickValue = map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
    }
    return JoystickValue;
}

void Read_Joystick()
{
//	#define analogRead_DMA analogRead
    CTRL.Left.X = JS_L.X = JoystickMap(analogRead_DMA(JSL_X_Pin - 2), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    CTRL.Left.Y = JS_L.Y = JoystickMap(analogRead_DMA(JSL_Y_Pin - 2), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    CTRL.Right.X = JS_R.X = JoystickMap(analogRead_DMA(JSR_X_Pin), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);
    CTRL.Right.Y = JS_R.Y = JoystickMap(analogRead_DMA(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);

    if(MPU_State)
    {
        if(from_MPU_LX) CTRL.Left.X = *from_MPU_LX;
		if(from_MPU_LY) CTRL.Left.Y = *from_MPU_LY;
		if(from_MPU_RX) CTRL.Right.X = *from_MPU_RX;
		if(from_MPU_RY) CTRL.Right.Y = *from_MPU_RY;
    }

    CTRL.KnobLimit.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    CTRL.KnobLimit.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
//    if(digitalRead(SPDT_Switch_Pin))
//    {
//        CTRL.KnobLimit.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
//        CTRL.KnobLimit.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
//    }
//    else
//    {
//        CTRL.KnobLimit.L = 0;
//        CTRL.KnobLimit.R = 0;
//    }
}

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEvent_Monitor();
}
