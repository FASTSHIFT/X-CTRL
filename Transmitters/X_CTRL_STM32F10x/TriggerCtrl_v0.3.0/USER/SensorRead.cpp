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
    CTRL.Left.Y = JS_L.Y = JoystickMap(analogRead(JSL_Y_Pin), JS_L.Ymin, JS_L.Ymid, JS_L.Ymax);
    CTRL.Right.Y= JS_R.Y = -JoystickMap(analogRead(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);
    CTRL.Left.X = JS_L.X = JoystickMap(analogRead(JSL_X_Pin), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    CTRL.Right.X = JS_R.X = -JoystickMap(analogRead(JSR_X_Pin), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);

    if(1/*digitalRead(SPDT_Switch_Pin)*/)
    {
        CTRL.KnobLimit.L = CtrlOutput_MaxValue - map(analogRead(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
        CTRL.KnobLimit.R = CtrlOutput_MaxValue - map(analogRead(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    }
    else
    {
        CTRL.KnobCab.L = map(analogRead(ADL_Pin), 0, ADC_MaxValue, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        CTRL.KnobCab.R = map(analogRead(ADR_Pin), 0, ADC_MaxValue, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

#include "DigitalFilter.h"
DigitalFilter dfBattUsage(13);

float Read_BattVoltage()
{
    float BattVoltage = (analogRead(BattSensor_Pin) / float(ADC_MaxValue)) * 3.3 * 2.0;
    LimitValue(BattVoltage, BattEmptyVoltage, BattFullVoltage);
    return (BattVoltage);
}

float BattUsage;
void Read_BattUsage()
{
    int BattUsage_Origin = fmap(Read_BattVoltage(), BattEmptyVoltage, BattFullVoltage, 0.0, 100.0);
    BattUsage = dfBattUsage.getSmooth(BattUsage_Origin);
}

void Task_SensorUpdate()
{
    Read_Joystick();
    ButtonEvent_Monitor();
	Read_BattUsage();
}
