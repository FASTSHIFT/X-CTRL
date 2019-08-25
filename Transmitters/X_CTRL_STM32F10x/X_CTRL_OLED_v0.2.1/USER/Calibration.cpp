#include "FileGroup.h"

void Calibrat_JSMP()
{
    JS_L.Xmp = 0, JS_L.Ymp = 0, JS_R.Xmp = 0, JS_R.Ymp = 0;
    for (uint8_t i = 0; i < 50; i++)
    {
        JS_L.Xmp = JS_L.Xmp + analogRead_DMA(JSL_X_Pin);
        JS_L.Ymp = JS_L.Ymp + analogRead_DMA(JSL_Y_Pin);
        JS_R.Xmp = JS_R.Xmp + analogRead_DMA(JSR_X_Pin);
        JS_R.Ymp = JS_R.Ymp + analogRead_DMA(JSR_Y_Pin);
        delay(10);
    }
    JS_L.Xmp = JS_L.Xmp / 50;
    JS_L.Ymp = JS_L.Ymp / 50;
    JS_R.Xmp = JS_R.Xmp / 50;
    JS_R.Ymp = JS_R.Ymp / 50;
}

void Calibrat_JSMaxMin()
{
    int l_xmax = 0, l_xmin = ADC_MaxValue, l_ymax = 0, l_ymin = ADC_MaxValue;
    int r_xmax = 0, r_xmin = ADC_MaxValue, r_ymax = 0, r_ymin = ADC_MaxValue;
    oled.clear();
    oled.setCursor(1, 0);
    oled.print("Calibrating...");
    Calibrat_JSMP();
    oled.setCursor(1, 1);
    oled.print("Turn JoySticks");
    oled.setCursor(1, 2);
    oled.print("Press 'OK'");
    delay(1000);
    oled.clear();
    while (1)
    {
        if (analogRead_DMA(JSL_X_Pin) > l_xmax)l_xmax = analogRead_DMA(JSL_X_Pin);
        if (analogRead_DMA(JSL_X_Pin) < l_xmin)l_xmin = analogRead_DMA(JSL_X_Pin);
        if (analogRead_DMA(JSL_Y_Pin) > l_ymax)l_ymax = analogRead_DMA(JSL_Y_Pin);
        if (analogRead_DMA(JSL_Y_Pin) < l_ymin)l_ymin = analogRead_DMA(JSL_Y_Pin);
        if (analogRead_DMA(JSR_X_Pin) > r_xmax)r_xmax = analogRead_DMA(JSR_X_Pin);
        if (analogRead_DMA(JSR_X_Pin) < r_xmin)r_xmin = analogRead_DMA(JSR_X_Pin);
        if (analogRead_DMA(JSR_Y_Pin) > r_ymax)r_ymax = analogRead_DMA(JSR_Y_Pin);
        if (analogRead_DMA(JSR_Y_Pin) < r_ymin)r_ymin = analogRead_DMA(JSR_Y_Pin);

        oled.setCursor(0, 0), oled.print("MAX:");
        oled.setCursor(5, 0), oled.print("LX:"), oled.print(l_xmax), NumClear(l_xmax, 4);
        oled.setCursor(5, 1), oled.print("LY:"), oled.print(l_ymax), NumClear(l_ymax, 4);
        oled.setCursor(5, 2), oled.print("RX:"), oled.print(r_xmax), NumClear(r_xmax, 4);
        oled.setCursor(5, 3), oled.print("RY:"), oled.print(r_ymax), NumClear(r_ymax, 4);

        oled.setCursor(0, 4), oled.print("MIN:");
        oled.setCursor(5, 4), oled.print("LX:"), oled.print(l_xmin), NumClear(l_xmin, 4);
        oled.setCursor(5, 5), oled.print("LY:"), oled.print(l_ymin), NumClear(l_ymin, 4);
        oled.setCursor(5, 6), oled.print("RX:"), oled.print(r_xmin), NumClear(r_xmin, 4);
        oled.setCursor(5, 7), oled.print("RY:"), oled.print(r_ymin), NumClear(r_ymin, 4);

        Read_Keys();
        if (Key[k_ok])
        {
            JS_L.Xmax = l_xmax;
            JS_L.Xmin = l_xmin;
            JS_L.Ymax = l_ymax;
            JS_L.Ymin = l_ymin;

            JS_R.Xmax = r_xmax;
            JS_R.Xmin = r_xmin;
            JS_R.Ymax = r_ymax;
            JS_R.Ymin = r_ymin;

            Key[k_ok] = 0;
            oled.clear();
            oled.setCursor(1, 0);
            oled.print("Calibrat Done");
            break;
        }
        else if (Key[k_back])
        {
            oled.clear();
            oled.setCursor(1, 0);
            oled.print("Calibrat Exit");
            break;
        }
    }
    delay(1000);
    oled.clear();
}

void Calibrat_ServoMP(int XmpMin, int XmpMax, int YmpMin, int YmpMax)
{
    int xmp = ServoPos.Xmp, ymp = ServoPos.Ymp;
    uint8_t MenuCursor = 1, MenuSetValue = Off, MenuSetValue_Cursor;

    oled.clear();

    while (1)
    {
        Read_Keys();

        if (Key[k_up] && MenuSetValue == 0)MenuCursor--;
        if (Key[k_down] && MenuSetValue == 0)MenuCursor++;
        if (MenuCursor < 1) MenuCursor = 3;
        if (MenuCursor > 3) MenuCursor = 1;

        if (MenuSetValue == Off && MenuCursor == 3 && Key[k_ok])
        {
            ServoPos.Xmp = xmp;
            ServoPos.Ymp = ymp;
            oled.setCursor(2, 4);
            oled.print("Calibrat Done");
            break;
        }

        if(Key[k_back])
        {
            if(MenuSetValue == Off)
            {
                oled.setCursor(2, 4);
                oled.print("Calibrat Exit");
                break;
            }
            else MenuSetValue = Off;
        }

        if (Key[k_ok] && MenuSetValue == 0)
        {
            MenuSetValue = 1;
            MenuSetValue_Cursor = MenuCursor;
        }

        if (MenuSetValue)
        {
            if(MenuSetValue_Cursor == 1)
            {
                if (Key[k_up] && xmp < XmpMax) xmp++;
                if (Key[k_down] && xmp > XmpMin) xmp--;
            }
            else if(MenuSetValue_Cursor == 2)
            {
                if (Key[k_up] && ymp < YmpMax) ymp++;
                if (Key[k_down] && ymp > YmpMin) ymp--;
            }
        }

        oled.setCursor(2, 0), oled.print("Servo Pos:");
        oled.setCursor(2, 1), oled.printf("X = %3d", xmp);
        oled.setCursor(2, 2), oled.printf("Y = %3d", ymp);
        oled.setCursor(2, 3), oled.print("Save");

        for(uint8_t i = 1; i <= 3; i++)
        {
            oled.setCursor(0, i);
            if(i == MenuCursor)
            {
                oled.drawBitmap(Triangle);
                if(MenuSetValue) oled.drawBitmap(Triangle_UD);
                else oled.print(' ');
            }
            else oled.print("  ");
        }
        KeyClear();


        //Exchg();
        //if (millis() >= send_time)SEND(), send_time = millis() + CTRL.SendFreq;
    }
    delay(1000);
    oled.clear();
    KeyClear();
}

void Calibrat_MPU6050()
{
    oled.clear();
    mpu.initialize();
    uint8_t Now_Silent_Mode = Silent_Mode;
    Silent_Mode = On;

    while (1)
    {
        Read_Keys();
        Read_Joystick();
        Read_MPU6050(MPU_ReadFreq);
        oled.setCursor(1, 0);
        oled.print("--MPU STATE--");

        oled.setCursor(1, 1);
        oled.printf("Roll :%6d", MPU_Roll);
        oled.setCursor(1, 2);
        oled.printf("Pitch:%6d", MPU_Pitch);

        oled.setCursor(1, 3);
        oled.printf("Limit: %5d", MPU_Max);
        oled.setCursor(1, 4);
        oled.printf("Freq :%3dms", MPU_ReadFreq);

        oled.setCursor(1, 5);
        oled.print("Attach:");
        oled.setCursor(1, 6);
        oled.print("Roll -> JS_");
        if(MPU_Roll_Attach == &(JS_L.X))
        {
            oled.print("L.X");
        }
        else
        {
            oled.print("R.X");
        }

        oled.setCursor(1, 7);
        oled.print("Pitch-> JS_");
        if(MPU_Pitch_Attach == &(JS_L.Y))
        {
            oled.print("L.Y");
        }
        else
        {
            oled.print("R.Y");
        }

        if(Key[k_up])
        {
            if(Key[k_l] || Key[k_r])
            {
                if(MPU_Max < 20000)MPU_Max++;
            }
            else
            {
                if(MPU_ReadFreq < 999)MPU_ReadFreq++;
            }
        }
        if(Key[k_down])
        {
            if(Key[k_l] || Key[k_r])
            {
                if(MPU_Max > 200)MPU_Max--;
            }
            else
            {
                if(MPU_ReadFreq > 1)MPU_ReadFreq--;
            }
        }

        if (Key[k_ok])
        {
            if(ABS(JS_L.X) > 200 && ABS(MPU_Roll) > (MPU_Max * 0.8))
            {
                MPU_Roll_Attach = &(JS_L.X);
            }
            else if(ABS(JS_R.X) > 200 && ABS(MPU_Roll) > (MPU_Max * 0.8))
            {
                MPU_Roll_Attach = &(JS_R.X);
            }

            if(ABS(JS_L.Y) > 200 && ABS(MPU_Pitch) > (MPU_Max * 0.8))
            {
                MPU_Pitch_Attach = &(JS_L.Y);
            }
            else if(ABS(JS_R.Y) > 200 && ABS(MPU_Pitch) > (MPU_Max * 0.8))
            {
                MPU_Pitch_Attach = &(JS_R.Y);
            }

            MPU_Switch = 1;
            Key[k_ok] = 0;
            if(ABS(MPU_Roll) < 2000 && ABS(MPU_Pitch) < 2000)break;
        }
        if (Key[k_back])
        {
            MPU_Switch = 0;
            Key[k_back] = 0;
            break;
        }
        KeyClear();
    }
    oled.clear();
    Silent_Mode = Now_Silent_Mode;
}

void Init_NRF()
{
    oled.clear();
    oled.setCursor(1, 0), oled.print("Init NRF...");
    if (CTRL.Object == SPIDER)nrf.SetAddress(12, 34, 56, 78, 90);
    else nrf.SetAddress(10, 20, 30, 40, 50);
    nrf.init(TXRX_MODE, TX_buff, sizeof(TX_buff), sizeof(RX_buff));

    oled.setCursor(1, 1), oled.print("Set Arrdess:");
    oled.setCursor(1, 2);
    for (uint8_t i = 0; i < 5; i++)
    {
        oled.print(nrf.GetAddress(i));
        oled.print(' ');
    }

    uint8_t NRF_Status = nrf.Status();
    oled.setCursor(1, 3), oled.printf("Status:0x%x", NRF_Status);
    oled.setCursor(1, 4);
    if (NRF_Status != 0x00 && NRF_Status != 0xFF)
    {
        oled.print("Online");
    }
    else
    {
        oled.print("Offline");
    }
    delay(1500);
    oled.clear();
}
