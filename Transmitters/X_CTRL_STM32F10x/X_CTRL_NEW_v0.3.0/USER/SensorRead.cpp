#include "FileGroup.h"

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
    CTRL.Left.Y = JS_L.Y = JoystickMap(analogRead_DMA(JSL_Y_Pin), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    CTRL.Right.Y = JS_R.Y = JoystickMap(analogRead_DMA(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);
    CTRL.Left.X = JS_L.X = JoystickMap(analogRead_DMA(JSL_X_Pin), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    CTRL.Right.X = JS_R.X = JoystickMap(analogRead_DMA(JSR_X_Pin), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);

    if(digitalRead(SPDT_Switch_Pin))
    {
        CTRL.KnobLimit.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
        CTRL.KnobLimit.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    }
    else
    {
        CTRL.KnobCab.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        CTRL.KnobCab.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
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

float Read_BattVoltage()
{
    float BattVoltage = (analogRead_DMA(BattSensor_Pin) / float(ADC_MaxValue)) * 3.3 * 2.0;
    LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

#include "DigitalFilter.h"
DigitalFilter dfBattUsage(13);

float BattUsage;
void Read_BattUsage()
{
    int BattUsage_Origin = fmap(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0, 100.0);
    BattUsage = dfBattUsage.getSmooth(BattUsage_Origin);
}

void Task_SensorUpdate()
{
    Read_Joystick();
    BottonEvent_Monitor();
    Encoder_Monitor();
    Read_BattUsage();
}
