#include "FileGroup.h"
#include "DigitalFilter.h"

Joystick_t JS_L;
Joystick_t JS_R;

static int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    if ((ADC_Value - MP) >= 0)
        return map(ADC_Value, MP, MAX, 0, CtrlOutput_MaxValue);
    else
        return map(ADC_Value, MIN, MP, -CtrlOutput_MaxValue, 0);
}

static void Read_Joystick()
{
    extern int16_t *from_MPU_LX, *from_MPU_LY, *from_MPU_RX, *from_MPU_RY;
    CTRL.Left.Y = JS_L.Y = JoystickMap(analogRead_DMA(JSL_Y_Pin), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    CTRL.Right.Y = JS_R.Y = JoystickMap(ADC_MaxValue - analogRead_DMA(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);
    CTRL.Left.X = JS_L.X = JoystickMap(analogRead_DMA(JSL_X_Pin), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    CTRL.Right.X = JS_R.X = JoystickMap(ADC_MaxValue - analogRead_DMA(JSR_X_Pin), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);

    if(State_MPU)
    {
        if(from_MPU_LX) CTRL.Left.X = *from_MPU_LX;
        if(from_MPU_LY) CTRL.Left.Y = *from_MPU_LY;
        if(from_MPU_RX) CTRL.Right.X = *from_MPU_RX;
        if(from_MPU_RY) CTRL.Right.Y = *from_MPU_RY;
    }

    if(digitalRead(SPDT_Switch_Pin))
    {
        CTRL.KnobLimit.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
        CTRL.KnobLimit.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    }
    else
    {
        CTRL.KnobLimit.L = 0;
        CTRL.KnobLimit.R = 0;
    }
}

void Task_EncoderLED()
{
    static uint16_t Bright, Bright_Set;
    static uint32_t TimePoint;
    static uint8_t EncoderState_Last;
    if(digitalRead(EncoderKey_Pin) == 0 || digitalRead(EncoderA_Pin) != EncoderState_Last)
    {
        Bright_Set = 300;
        TimePoint = millis() + 500;
    }
    else if(millis() > TimePoint)
    {
        Bright_Set = 0;
    }

    if(Bright > Bright_Set)Bright -= 10;
    else if(Bright < Bright_Set)Bright += 20;

    EncoderState_Last = digitalRead(EncoderA_Pin);

    pwmWrite(EncoderLED_Pin, Bright);
}

float BattVoltage,BattUsage;
static DigitalFilter<float> dfBattUsage(13);
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15

static float Read_BattVoltage()
{
    BattVoltage = (analogRead_DMA(BattSensor_Pin) / float(ADC_MaxValue)) * 3.3 * 2.0;
    __LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

static void Read_BattUsage()
{
    BattUsage = dfBattUsage.getSmooth(__Map(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0, 100.0));
}

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEventMonitor();
    EncoderMonitor();
    Read_BattUsage();
}
