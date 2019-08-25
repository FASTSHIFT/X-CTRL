#include "FileGroup.h"

void LCD_SHOW(void)
{
  static unsigned long LCD_time;//LCD刷新时间点
  static String NOW_ACT;
  if (millis() >= LCD_time)
  {
    //***********************连接状态、模式显示*************************//
    lcd.setCursor(1, 0);
    lcd.write(0);
    if (millis() < recv_time)
    {
      lcd.write(1);
      if (ring_flag != 2)RING(2);
    }
    else
    {
      if (CTRL_State == Off)lcd.print('_');
      else lcd.print('x');
      if (ring_flag != 1)RING(1);
    }

    lcd.setCursor(3, 0);
    if (CTRL_module == nRF)lcd.print('N');
    else lcd.print('U');
    lcd.print(ctrl_md);
    if (CTRL_object == CAR)lcd.print('c');
    else if (CTRL_object == TANK)lcd.print('t');
    else if (CTRL_object == CPTR)lcd.print('p');
    else if (CTRL_object == SPIDER)lcd.print('s');
    else lcd.print('_');

    //************************其他信息************************//
    if (CTRL_object == CAR || CTRL_object == TANK)
    {
      lcd.setCursor(6, 0);
      if (BATT_V < 10)lcd.print(' ');
      lcd.print(BATT_V, 1);
      lcd.print('V');

      lcd.setCursor(11, 0);
      //lcd.print("CPU:");
      lcd.print(CPU_Temp, 1);
      lcd.write(2);
      lcd.print(' ');

      lcd.setCursor(1, 1);
      lcd.print("L:"), lcd.print(L_PWM), lcd.print("   ");
      lcd.setCursor(9, 1);
      lcd.print("R:"), lcd.print(R_PWM), lcd.print("   ");
    }
    else if (CTRL_object == CPTR)
    {
      lcd.setCursor(6, 0);
      if (BATT_V < 10)lcd.print(' ');
      lcd.print(BATT_V, 1);
      lcd.print('V');

      lcd.setCursor(11, 0);
      //lcd.print("CPU:");
      lcd.print(CPU_Temp, 1);
      lcd.write(2);
      lcd.print(' ');

      lcd.setCursor(1, 1);
      lcd.print("X:"); lcd.print(L_X), lcd.print("   ");
      lcd.setCursor(9, 1);
      lcd.print("Y:"); lcd.print(L_Y), lcd.print("   ");
    }
    else if (CTRL_object == SPIDER)
    {
      int x_agl, y_agl;
      int offset, StepL;
      int SPD, High;
      if (NOW_ACT != "SLEP" && (BT_DOWN() || abs(L_X) > 0 || abs(L_Y) > 0 || abs(R_X) > 0 || abs(R_Y) > 0))
      {
        if (abs(L_X) > 0 || abs(L_Y) > 0 || k_l || k_r)
        {
          NOW_ACT = " MV ";
          offset = abs(L_X) + abs(L_Y);
          if (k_l)NOW_ACT = "TLFT";
          else if (k_r)NOW_ACT = "TRGT";
        }
        if (NOW_ACT == "STOP" && (abs(R_X) > 0 || abs(R_Y) > 0))NOW_ACT = " MR ";
      }
      else if (NOW_ACT != "SLEP")NOW_ACT = "STOP";

      if (k_ok)NOW_ACT = "SLEP";
      if (k_jsl)NOW_ACT = "STOP";

      StepL = map(offset, 0, 510, 5, 30);
      SPD = map(L_CRE, 0, 1023, 0, 100);
      High = map(R_CRE, 0, 1023, 10, 50);
      x_agl = map(R_Y, -255, 255, -10, 10);
      y_agl = map(R_X, -255, 255, -10, 10);  

      lcd.setCursor(6, 0);
      lcd.print(NOW_ACT);

      lcd.setCursor(11, 0);
      lcd.print("S:"), lcd.print(SPD), lcd.print("  ");

      lcd.setCursor(1, 1);
      lcd.print("Sp"), lcd.print(StepL);
      if (StepL < 10)lcd.print(' ');

      lcd.setCursor(5, 1);
      lcd.print("H"), lcd.print(High);

      lcd.setCursor(9, 1);
      if (abs(x_agl) < 10)
      {
        if (x_agl < 0)lcd.print(' ');
        else lcd.print("  ");
      }
      else if (x_agl == 10)lcd.print(' ');
      lcd.print(x_agl);

      lcd.setCursor(12, 1);
      if (abs(y_agl) < 10)
      {
        if (y_agl < 0)lcd.print(' ');
        else lcd.print("  ");
      }
      else if (y_agl == 10)lcd.print(' ');
      lcd.print(y_agl);
    }

    LCD_time = millis() + 100;
  }
}