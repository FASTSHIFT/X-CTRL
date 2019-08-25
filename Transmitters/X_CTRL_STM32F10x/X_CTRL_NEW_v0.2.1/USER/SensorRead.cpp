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
    CTRL.Right.Y= JS_R.Y = JoystickMap(analogRead_DMA(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymid, JS_R.Ymax);
    CTRL.Left.X = JS_L.X = JoystickMap(analogRead_DMA(JSL_X_Pin), JS_L.Xmin, JS_L.Xmid, JS_L.Xmax);
    CTRL.Right.X= JS_R.X = JoystickMap(analogRead_DMA(JSR_X_Pin), JS_R.Xmin, JS_R.Xmid, JS_R.Xmax);

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

void EncoderLED_Handler()
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

void Read_Encoder()
{
    static boolean encoderALast = LOW;
    boolean encoderA = digitalRead(EncoderA_Pin);
    if ((encoderALast == HIGH) && (encoderA == LOW))
    {
        if (digitalRead(EncoderB_Pin) == LOW)
        {
            CTRL.Key |= BT_UP;
        }
        else
        {
            CTRL.Key |= BT_DOWN;
        }
    }
    encoderALast = encoderA;

    static uint8_t KeyState_Last;
    static uint32_t PressKey_TimePoint;
    uint8_t KeyState = !digitalRead(EncoderKey_Pin);

    if(KeyState == 1 && KeyState_Last == 0)//Press key ,mark TimePoint
    {
        PressKey_TimePoint = millis() + 500;
    }
    else if(KeyState == 0 && KeyState_Last == 1)//Release key,check TimePoint
    {
        if(millis() < PressKey_TimePoint)//Short press
        {
            CTRL.Key |= BT_OK;
        }
        else//Long Press
        {
            CTRL.Key |= BT_BACK;
        }
    }
    KeyState_Last = KeyState;//Backup key state
}

uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

void Read_Keys()
{
//    if(MenuState == OPT)KeyClear();
    uint8_t BottonValue = Get_HC165_Value();

    static uint32_t PressKey_TimePoint;
    static bool IS_Pressed = false;

    if (!IS_Pressed && BottonValue)
    {
        if (bitRead(BottonValue, 4))CTRL.Key |= BT_UP;
        if (bitRead(BottonValue, 7))CTRL.Key |= BT_DOWN;
        if (bitRead(BottonValue, 6))CTRL.Key |= BT_OK;
        if (bitRead(BottonValue, 5))CTRL.Key |= BT_BACK;
        if (bitRead(BottonValue, 1))CTRL.Key |= BT_L1;
        if (bitRead(BottonValue, 2))CTRL.Key |= BT_R1;
        if (bitRead(BottonValue, 0))CTRL.Key |= BT_L2;
        if (bitRead(BottonValue, 3))CTRL.Key |= BT_R2;
        IS_Pressed = true;
        PressKey_TimePoint = millis() + 500;
    }
    else if (IS_Pressed && millis() >= PressKey_TimePoint && BottonValue)
    {
        if (bitRead(BottonValue, 4))CTRL.Key |= BT_UP;
        if (bitRead(BottonValue, 7))CTRL.Key |= BT_DOWN;
        if (bitRead(BottonValue, 6))CTRL.Key |= BT_OK;
        if (bitRead(BottonValue, 5))CTRL.Key |= BT_BACK;
        if (bitRead(BottonValue, 1))CTRL.Key |= BT_L1;
        if (bitRead(BottonValue, 2))CTRL.Key |= BT_R1;
        if (bitRead(BottonValue, 0))CTRL.Key |= BT_L2;
        if (bitRead(BottonValue, 3))CTRL.Key |= BT_R2;
    }
    else if (IS_Pressed && !BottonValue)
    {
        CTRL.Key = 0x00;
        IS_Pressed = false;
    }
    Read_Encoder();
}

float Read_BattVoltage()
{
    return ((analogRead_DMA(BattSensor_Pin) / float(ADC_MaxValue)) * 3.3 * 2.0);
}

void Task_SensorUpdate()
{
	Read_Joystick();
	Read_Encoder();
	EncoderLED_Handler();
	Read_Keys();
}
