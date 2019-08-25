#include "FileGroup.h"

void LCD_Display()
{
    static uint32_t LCD_TimePoint;//LCD刷新时间点
    if (millis() >= LCD_TimePoint)
    {
        ConnectInfo();

        if (CTRL_Object == CAR || CTRL_Object == TANK) CarTankInfo();
        else if (CTRL_Object == CPTR) ComputerInfo();
        else if (CTRL_Object == SPIDER) SpiderInfo();
        else if (CTRL_Object == CRAWLER) CrawlerInfo();

        LCD_TimePoint = millis() + 100;
    }
}

void ConnectInfo()
{
    char CTRL_Object_Char[] = {'c', 't', 'p', 's', 'w'};
    lcd.setCursor(1, 0);
    lcd.write(0);
    if (millis() < Recv_TimePoint)
    {
        lcd.write(1);
        if (RingLast != 2)BuzzRing(2);
    }
    else
    {
        if (CTRL_State == Off)lcd.print('_');
        else lcd.print('x');
        if (RingLast != 1)BuzzRing(1);
    }

    lcd.setCursor(3, 0);
    if (CTRL_Module == Module_NRF)lcd.print('N');
    else lcd.print('U');
    lcd.print(CtrlMode);
    lcd.print(CTRL_Object_Char[CTRL_Object]);
}



void CarTankInfo()
{
    lcd.setCursor(6, 0);
    if (Slave_BattVol < 10)lcd_Space(1);
    lcd.print(Slave_BattVol, 1);
    lcd.print('V');

    lcd.setCursor(11, 0);
    lcd.print(Slave_Temp, 1);
    lcd.write(2);
    lcd_Space(1);

    lcd.setCursor(1, 1);
    lcd.print("L:"), lcd.print(L_PWM), lcd_Space(3);
    lcd.setCursor(9, 1);
    lcd.print("R:"), lcd.print(R_PWM), lcd_Space(3);
    //  OLED_Display();
//    if (CTRL_Module == Module_NRF)
//    {
//        HMI_Display();
//    }
}

void ComputerInfo()
{
    lcd.setCursor(6, 0);
    if (Slave_BattVol < 10)lcd_Space(1);
    lcd.print(Slave_BattVol, 1);
    lcd.print('V');

    lcd.setCursor(11, 0);
    lcd.print(Slave_Temp, 1);
    lcd.write(2);
    lcd_Space(1);

    lcd.setCursor(1, 1);
    lcd.print("X:");
    lcd.print(JS_L.X), lcd_Space(3);
    lcd.setCursor(9, 1);
    lcd.print("Y:");
    lcd.print(JS_L.Y), lcd_Space(3);
}

void SpiderInfo()
{
    static String NOW_ACT;
    int16_t x_agl, y_agl;
    int16_t offset, StepL;
    int16_t SPD, High;
    if (NOW_ACT != "SLEP" && (BT_DOWN() || abs(JS_L.X) > 0 || abs(JS_L.Y) > 0 || abs(JS_R.X) > 0 || abs(JS_R.Y) > 0))
    {
        if (abs(JS_L.X) > 0 || abs(JS_L.Y) > 0 || Key[k_l] || Key[k_r])
        {
            NOW_ACT = " MV ";
            offset = abs(JS_L.X) + abs(JS_L.Y);
            if (Key[k_l])NOW_ACT = "TLFT";
            else if (Key[k_r])NOW_ACT = "TRGT";
        }
        if (NOW_ACT == "STOP" && (abs(JS_R.X) > 0 || abs(JS_R.Y) > 0))NOW_ACT = " MR ";
    }
    else if (NOW_ACT != "SLEP")NOW_ACT = "STOP";

    if (Key[k_ok])NOW_ACT = "SLEP";
    if (Key[k_jsl])NOW_ACT = "STOP";

    StepL = map(offset, 0, 510, 5, 30);
    SPD = map(L_CRE, 0, ADC_MaxValue, 0, 100);
    High = map(R_CRE, 0, ADC_MaxValue, 10, 50);
    x_agl = map(JS_R.Y, -PWM_MaxValue, PWM_MaxValue, -10, 10);
    y_agl = map(JS_R.X, -PWM_MaxValue, PWM_MaxValue, -10, 10);

    lcd.setCursor(6, 0);
    lcd.print(NOW_ACT);

    lcd.setCursor(11, 0);
    lcd.print("S:"), lcd.print(SPD), lcd_Space(2);

    lcd.setCursor(1, 1);
    lcd.print("Sp"), lcd.print(StepL);
    if (StepL < 10)lcd_Space(1);

    lcd.setCursor(5, 1);
    lcd.print("H"), lcd.print(High);

    lcd.setCursor(9, 1);
    if (abs(x_agl) < 10)
    {
        if (x_agl < 0)lcd_Space(1);
        else lcd_Space(2);
    }
    else if (x_agl == 10)lcd_Space(1);
    lcd.print(x_agl);

    lcd.setCursor(12, 1);
    if (abs(y_agl) < 10)
    {
        if (y_agl < 0)lcd_Space(1);
        else lcd_Space(2);
    }
    else if (y_agl == 10)lcd_Space(1);
    lcd.print(y_agl);
}

void CrawlerInfo()
{
    CarTankInfo();
    OLED_Display();
}

void lcd_Space(uint8_t num)
{
    while (num--)
    {
        lcd.print(' ');
    }
}

