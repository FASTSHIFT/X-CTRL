#include "FileGroup.h"

void Calibrat_JSMP()
{
    JS_L.Xmp = 0, JS_L.Ymp = 0, JS_R.Xmp = 0, JS_R.Ymp = 0;
    for (uint8_t i = 0; i < 50; i++)
    {
        JS_L.Xmp = JS_L.Xmp + analogRead(JSL_X_Pin);
        JS_L.Ymp = JS_L.Ymp + analogRead(JSL_Y_Pin);
        JS_R.Xmp = JS_R.Xmp + analogRead(JSR_X_Pin);
        JS_R.Ymp = JS_R.Ymp + analogRead(JSR_Y_Pin);
        delay(10);
    }
    JS_L.Xmp = JS_L.Xmp / 50;
    JS_L.Ymp = JS_L.Ymp / 50;
    JS_R.Xmp = JS_R.Xmp / 50;
    JS_R.Ymp = JS_R.Ymp / 50;
}

void Calibrat_JSMaxMin()
{
    int16_t l_xmax = 0, l_xmin = ADC_MaxValue, l_ymax = 0, l_ymin = ADC_MaxValue;
    int16_t r_xmax = 0, r_xmin = ADC_MaxValue, r_ymax = 0, r_ymin = ADC_MaxValue;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Calibrating..."));
    Calibrat_JSMP();
    lcd.setCursor(0, 0);
    lcd.print(F("Turn L_JoySticks"));
    lcd.setCursor(1, 1);
    lcd.print(F("and Press OK"));
    delay(1000);
    lcd.clear();
    while (1)
    {
        if (analogRead(JSL_X_Pin) > l_xmax)l_xmax = analogRead(JSL_X_Pin);
        if (analogRead(JSL_X_Pin) < l_xmin)l_xmin = analogRead(JSL_X_Pin);
        if (analogRead(JSL_Y_Pin) > l_ymax)l_ymax = analogRead(JSL_Y_Pin);
        if (analogRead(JSL_Y_Pin) < l_ymin)l_ymin = analogRead(JSL_Y_Pin);

        lcd.setCursor(0, 0);
        lcd.print(F("Xmax:"));
        lcd.print(l_xmax);
        lcd.setCursor(9, 0);
        lcd.print(F("min:"));
        lcd.print(l_xmin);
        lcd_Space(3);

        lcd.setCursor(0, 1);
        lcd.print(F("Ymax:"));
        lcd.print(l_ymax);
        lcd.setCursor(9, 1);
        lcd.print(F("min:"));
        lcd.print(l_ymin);
        lcd_Space(3);

        Read_Keys();
        if (Key[k_ok] == 1)
        {
            JS_L.Xmax = l_xmax;
            JS_L.Xmin = l_xmin;
            JS_L.Ymax = l_ymax;
            JS_L.Ymin = l_ymin;
            Key[k_ok] = 0;
            break;
        }
        else if (Key[k_back] == 1)break;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Turn R_JoySticks"));
    lcd.setCursor(1, 1);
    lcd.print(F("and Press OK"));
    delay(1000);
    lcd.clear();
    while (1)
    {
        if (analogRead(JSR_X_Pin) > r_xmax)r_xmax = analogRead(JSR_X_Pin);
        if (analogRead(JSR_X_Pin) < r_xmin)r_xmin = analogRead(JSR_X_Pin);
        if (analogRead(JSR_Y_Pin) > r_ymax)r_ymax = analogRead(JSR_Y_Pin);
        if (analogRead(JSR_Y_Pin) < r_ymin)r_ymin = analogRead(JSR_Y_Pin);

        lcd.setCursor(0, 0);
        lcd.print(F("Xmax:"));
        lcd.print(r_xmax);
        lcd.setCursor(9, 0);
        lcd.print(F("min:"));
        lcd.print(r_xmin);
        lcd_Space(3);

        lcd.setCursor(0, 1);
        lcd.print(F("Ymax:"));
        lcd.print(r_ymax);
        lcd.setCursor(9, 1);
        lcd.print(F("min:"));
        lcd.print(r_ymin);
        lcd_Space(3);

        Read_Keys();
        if (Key[k_ok] == 1)
        {
            JS_R.Xmax = r_xmax;
            JS_R.Xmin = r_xmin;
            JS_R.Ymax = r_ymax;
            JS_R.Ymin = r_ymin;
            Key[k_ok] = 0;
            break;
        }
        else if (Key[k_back] == 1)break;
    }
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Calibrat End"));
    delay(1000);
    lcd.clear();
}

void Calibrat_ServoMP(int16_t XmpMin, int16_t XmpMax, int16_t YmpMin, int16_t YmpMax)
{
    int16_t xmp = ServoPos.Xmp, ymp = ServoPos.Ymp;
    uint32_t send_time;

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Adjust Servo"));
    lcd.setCursor(1, 1);
    lcd.print(F("and Press OK"));
    delay(1000);
    lcd.clear();

    while (1)
    {
        Read_Keys();
        Read_Joystick();

        if ((Key[k_r] || JS_R.X > 20) && xmp > XmpMin)Key[k_l] = 0, xmp--;
        if ((Key[k_l] || JS_R.X < -20) && xmp < XmpMax)Key[k_r] = 0, xmp++;
        if ((Key[k_up] || JS_R.Y > 20) && ymp > YmpMin)Key[k_up] = 0, ymp--;
        if ((Key[k_down] || JS_R.Y < -20) && ymp < YmpMax)Key[k_down] = 0, ymp++;

        lcd.setCursor(1, 0);
        lcd.print(F("Servo:"));
        lcd.setCursor(1, 1);
        lcd.print(F("X="));
        lcd.print(xmp);
        lcd_Space(2);
        lcd.setCursor(9, 1);
        lcd.print(F("Y="));
        lcd.print(ymp);
        lcd_Space(2);

        ServoPos.X = xmp;
        ServoPos.Y = ymp;
        TX_DataLoad();
        if (millis() >= send_time)SendData(), send_time = millis() + SendFreq;
        if (Key[k_ok])
        {
            ServoPos.Xmp = xmp;
            ServoPos.Ymp = ymp;
            Key[k_ok] = 0;
            break;
        }
        else if (Key[k_back])break;
    }
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Calibrat End"));
    delay(1000);
    lcd.clear();
}

#ifdef USE_MPU6050
void Calibrat_MPU6050()
{
    lcd.clear();
    mpu.initialize();

    while (1)
    {
        Read_Keys();
        Read_MPU6050();
        lcd.setCursor(1, 0);
        lcd.print(F("Roll :"));
        lcd.print(-MPU_Roll);
        lcd_Space(3);
        lcd.setCursor(1, 1);
        lcd.print(F("Pitch:"));
        lcd.print(MPU_Pitch);
        lcd_Space(3);
        if (Key[k_ok])
        {
            MPU_Switch = 1;
            Key[k_ok] = 0;
            break;
        }
        if (Key[k_back])
        {
            MPU_Switch = 0;
            Key[k_back] = 0;
            break;
        }
    }
    lcd.clear();
}
#endif
