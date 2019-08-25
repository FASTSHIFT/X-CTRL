#include "FileGroup.h"

void LoadCtrlMode(uint8_t cmd)
{
    float l_cre = L_CRE / float(ADC_MaxValue);
    float r_cre = R_CRE / float(ADC_MaxValue);

    if (MPU_Switch == On && MenuState == INF)
    {
#ifdef USE_MPU6050
        if (CTRL_Object == CPTR )
        {
            JS_R.X = map(-MPU_Roll, -MPU_MAX, MPU_MAX, -PWM_MaxValue, PWM_MaxValue);
            JS_R.Y = map(MPU_Pitch, -MPU_MAX, MPU_MAX, -PWM_MaxValue, PWM_MaxValue);
        }
        else if ((CTRL_Object == CAR || CTRL_Object == CRAWLER))
        {
            if (cmd == 2)
            {
                JS_R.X = map(-MPU_Roll, -MPU_MAX, MPU_MAX, -PWM_MaxValue, PWM_MaxValue);
                JS_L.Y = map(MPU_Pitch, -MPU_MAX, MPU_MAX, -PWM_MaxValue, PWM_MaxValue);
            }
            else if (cmd == 3)
            {
                JS_L.X = map(-MPU_Roll, -MPU_MAX, MPU_MAX, -PWM_MaxValue, PWM_MaxValue);
                JS_L.Y = map(MPU_Pitch, -MPU_MAX, MPU_MAX, -PWM_MaxValue, PWM_MaxValue);
            }
        }
#endif
    }

    switch (cmd)
    {
    case 1:
        L_PWM = JS_L.Y * l_cre, R_PWM = JS_R.Y * r_cre;
        break;

    case 2:
        if (JS_L.Y >= 0)
        {
            L_PWM = JS_L.Y * l_cre + JS_R.X * r_cre;
            R_PWM = JS_L.Y * l_cre - JS_R.X * r_cre;
        }
        else
        {
            L_PWM = JS_L.Y * l_cre - JS_R.X * r_cre;
            R_PWM = JS_L.Y * l_cre + JS_R.X * r_cre;
        }
        break;

    case 3:
        if (JS_L.Y >= 0)
        {
            L_PWM = JS_L.Y * l_cre + JS_L.X * l_cre * 0.5;
            R_PWM = JS_L.Y * l_cre - JS_L.X * l_cre * 0.5;
        }
        else
        {
            L_PWM = JS_L.Y * l_cre - JS_L.X * l_cre * 0.5;
            R_PWM = JS_L.Y * l_cre + JS_L.X * l_cre * 0.5;
        }

        //      if (JS_R.X >= 0) ServoPos.X = 180 - map(JS_R.X * r_cre, 0, 255, 180 - ServoPos.Xmp, 180);
        //      else if (JS_R.X < 0) ServoPos.X = 180 - map(JS_R.X * r_cre, -255, 0, 0, 180 - ServoPos.Xmp);
        //      if (JS_R.Y >= 0)ServoPos.Y = 180 - map(JS_R.Y * r_cre, 0, 255, 180 - ServoPos.Ymp, 180);
        //      else if (JS_R.Y < 0)ServoPos.Y = 180 - map(JS_R.Y * r_cre, -255, 0, 0, 180 - ServoPos.Ymp);
        //ServoPos.Xmp = 180 - map(R_CRE, 0, ADC_MaxValue, 0, 180);

        if (JS_R.X >= 0)    ServoPos.X = 180 - PowMap(JS_R.X * r_cre, 0, PWM_MaxValue, 180 - ServoPos.Xmp, 180, 0.1, 3);
        else if (JS_R.X < 0)ServoPos.X = 180 - PowMap(JS_R.X * r_cre, -PWM_MaxValue, 0, 0, 180 - ServoPos.Xmp, 3, 0.1);

        if (JS_R.Y >= 0)    ServoPos.Y = 180 - PowMap(JS_R.Y * r_cre, 0, PWM_MaxValue, 180 - ServoPos.Ymp, 180, 0.1, 3);
        else if (JS_R.Y < 0)ServoPos.Y = 180 - PowMap(JS_R.Y * r_cre, -PWM_MaxValue, 0, 0, 180 - ServoPos.Ymp, 0.1, 3);

        B_PWM = -JS_R.X;
        break;
    }
    if (cmd != 3 && MenuState == INF && (CTRL_Object == CAR || CTRL_Object == TANK || CTRL_Object == CRAWLER))
    {
        static uint32_t PosAdd_Time;
        uint8_t UseFullAngle_Y[3] = {ServoPos.Ymp, ServoPos.Ymp - 90, 0};
        static uint8_t Mode = 0;
        if (millis() >= PosAdd_Time)
        {
            if (Key[k_down])
            {
                Key[k_down] = 0;
                Mode = 0;
                ServoPos.Y++;
            }
            if (Key[k_up])
            {
                Key[k_up] = 0;
                Mode = 0;
                ServoPos.Y--;
            }
            if (Key[k_r])
            {
                Key[k_l] = 0;
                Mode = 0;
                ServoPos.X--;
                B_PWM--;
            }
            if (Key[k_l])
            {
                Key[k_r] = 0;
                Mode = 0;
                ServoPos.X++;
                B_PWM++;
            }
            if (Key[k_r] + Key[k_l] == 0)B_PWM = 0;

            if (Key[k_ok])
            {
                Key[k_ok] = 0;
                if (Mode > 2)Mode = 0;
                ServoPos.X = ServoPos.Xmp;
                ServoPos.Y = UseFullAngle_Y[Mode];
                Mode++;
            }

            PosAdd_Time = millis() + 20;
        }
    }

    Limit(L_PWM, -PWM_MaxValue, PWM_MaxValue);
    Limit(R_PWM, -PWM_MaxValue, PWM_MaxValue);
    Limit(B_PWM, -PWM_MaxValue, PWM_MaxValue);
    Limit(ServoPos.X, 0, 180);
    Limit(ServoPos.Y, 0, 180);
}

