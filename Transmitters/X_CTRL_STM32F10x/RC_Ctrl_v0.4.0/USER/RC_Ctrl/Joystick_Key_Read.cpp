#include "FileGroup.h"

void Read_Joystick()
{
    JS_L.Y = JoystickMap(analogRead(JSL_Y_Pin), JS_L.Ymin, JS_L.Ymp, JS_L.Ymax);
    JS_R.Y = JoystickMap(analogRead(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymp, JS_R.Ymax);
    JS_L.X = JoystickMap(analogRead(JSL_X_Pin), JS_L.Xmin, JS_L.Xmp, JS_L.Xmax);
    JS_R.X = JoystickMap(analogRead(JSR_X_Pin), JS_R.Xmin, JS_R.Xmp, JS_R.Xmax);

    L_CRE = analogRead(ADL_Pin);
    R_CRE = analogRead(ADR_Pin);
}

int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
{
    int16_t JoystickValue;
    if ((ADC_Value - MP) >= 0)
    {
        JoystickValue = map(ADC_Value, MP, MAX, 0, PWM_MaxValue);
    }
    else
    {
        JoystickValue = map(ADC_Value, MIN, MP, -PWM_MaxValue, 0);
    }
    return JoystickValue;
}

void Read_Keys()
{
    static uint32_t PressKey_TimePoint;
    static uint8_t press_flag = 0;

    if (press_flag == 0 && (BT_DOWN() != 0))
    {
        delay(10);
        if ((BT_DOWN() != 0))
        {
            if (Silent_Mode == Off && MenuState == OPT)BuzzRing(5);
            if (digitalRead(KEY_UP_Pin) == 0)Key[k_up] = 1;
            if (digitalRead(KEY_DOWN_Pin) == 0)Key[k_down] = 1;
            if (digitalRead(KEY_OK_Pin) == 0)Key[k_ok] = 1;
            if (digitalRead(KEY_BACK_Pin) == 0)Key[k_back] = 1;
            if (digitalRead(KEY_JSL_Pin) == 0)Key[k_jsl] = 1;
            if (digitalRead(KEY_JSR_Pin) == 0)Key[k_jsr] = 1;
            if (digitalRead(KEY_L_Pin) == 0)Key[k_l] = 1;
            if (digitalRead(KEY_R_Pin) == 0)Key[k_r] = 1;
            press_flag = 1;
            PressKey_TimePoint = millis() + 500;
        }
    }
    else if (press_flag == 1 && millis() >= PressKey_TimePoint && (BT_DOWN() != 0))
    {
        if (Silent_Mode == Off && MenuState == OPT)BuzzRing(5);
        if (digitalRead(KEY_UP_Pin) == 0)Key[k_up] = 1;
        if (digitalRead(KEY_DOWN_Pin) == 0)Key[k_down] = 1;
        if (digitalRead(KEY_OK_Pin) == 0)Key[k_ok] = 1;
        if (digitalRead(KEY_BACK_Pin) == 0)Key[k_back] = 1;
        if (digitalRead(KEY_JSL_Pin) == 0)Key[k_jsl] = 1;
        if (digitalRead(KEY_JSR_Pin) == 0)Key[k_jsr] = 1;
        if (digitalRead(KEY_L_Pin) == 0)Key[k_l] = 1;
        if (digitalRead(KEY_R_Pin) == 0)Key[k_r] = 1;
    }

    else if (press_flag == 1 && (BT_DOWN() == 0))
    {
        delay(10);
        if (BT_DOWN() == 0)
        {
            Key[k_up] = 0;
            Key[k_down] = 0;
            Key[k_ok] = 0;
            Key[k_back] = 0;
            Key[k_jsl] = 0;
            Key[k_jsr] = 0;
            Key[k_l] = 0;
            Key[k_r] = 0;

            press_flag = 0;
        }
    }
}

uint8_t BT_DOWN()
{
    return (!digitalRead(KEY_UP_Pin)   ||
            !digitalRead(KEY_DOWN_Pin) ||
            !digitalRead(KEY_OK_Pin)   ||
            !digitalRead(KEY_BACK_Pin) ||
            !digitalRead(KEY_JSL_Pin)  ||
            !digitalRead(KEY_JSR_Pin)  ||
            !digitalRead(KEY_L_Pin)    ||
            !digitalRead(KEY_R_Pin));
}
