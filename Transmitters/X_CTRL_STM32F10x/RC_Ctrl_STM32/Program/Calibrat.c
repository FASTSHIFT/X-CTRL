#include "FileGroup.h"

void Calibrat_JSMP(void)
{
  L_XMP = 0, L_YMP = 0, R_XMP = 0, R_YMP = 0;
  for (byte i = 0; i < 50; i++)
  {
    L_XMP = L_XMP + analogRead(JSL_X);
    L_YMP = L_YMP + analogRead(JSL_Y);
    R_XMP = R_XMP + analogRead(JSR_X);
    R_YMP = R_YMP + analogRead(JSR_Y);
    delay(10);
  }
  L_XMP = L_XMP / 50;
  L_YMP = L_YMP / 50;
  R_XMP = R_XMP / 50;
  R_YMP = R_YMP / 50;
}

void Calibrat_JSMaxMin(void)
{
  int l_xmax = 0, l_xmin = 1023, l_ymax = 0, l_ymin = 1023;
  int r_xmax = 0, r_xmin = 1023, r_ymax = 0, r_ymin = 1023;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Calibrating...");
  Calibrat_JSMP();
  lcd.setCursor(0, 0);
  lcd.print("Turn L_JoySticks");
  lcd.setCursor(1, 1);
  lcd.print("and Press OK");
  delay(1000);
  lcd.clear();
  while (1)
  {
    if (analogRead(JSL_X) > l_xmax)l_xmax = analogRead(JSL_X);
    if (analogRead(JSL_X) < l_xmin)l_xmin = analogRead(JSL_X);
    if (analogRead(JSL_Y) > l_ymax)l_ymax = analogRead(JSL_Y);
    if (analogRead(JSL_Y) < l_ymin)l_ymin = analogRead(JSL_Y);

    lcd.setCursor(0, 0);
    lcd.print("Xmax:");
    lcd.print(l_xmax);
    lcd.setCursor(9, 0);
    lcd.print("min:");
    lcd.print(l_xmin);
    lcd.print("   ");

    lcd.setCursor(0, 1);
    lcd.print("Ymax:");
    lcd.print(l_ymax);
    lcd.setCursor(9, 1);
    lcd.print("min:");
    lcd.print(l_ymin);
    lcd.print("   ");

    KEY_Read();
    if (k_ok == 1)
    {
      L_XMAX = l_xmax;
      L_XMIN = l_xmin;
      L_YMAX = l_ymax;
      L_YMIN = l_ymin;
      k_ok == 0;
      break;
    }
    else if (k_back == 1)break;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn R_JoySticks");
  lcd.setCursor(1, 1);
  lcd.print("and Press OK");
  delay(1000);
  lcd.clear();
  while (1)
  {
    if (analogRead(JSR_X) > r_xmax)r_xmax = analogRead(JSR_X);
    if (analogRead(JSR_X) < r_xmin)r_xmin = analogRead(JSR_X);
    if (analogRead(JSR_Y) > r_ymax)r_ymax = analogRead(JSR_Y);
    if (analogRead(JSR_Y) < r_ymin)r_ymin = analogRead(JSR_Y);

    lcd.setCursor(0, 0);
    lcd.print("Xmax:");
    lcd.print(r_xmax);
    lcd.setCursor(9, 0);
    lcd.print("min:");
    lcd.print(r_xmin);
    lcd.print("   ");

    lcd.setCursor(0, 1);
    lcd.print("Ymax:");
    lcd.print(r_ymax);
    lcd.setCursor(9, 1);
    lcd.print("min:");
    lcd.print(r_ymin);
    lcd.print("   ");

    KEY_Read();
    if (k_ok == 1)
    {
      R_XMAX = r_xmax;
      R_XMIN = r_xmin;
      R_YMAX = r_ymax;
      R_YMIN = r_ymin;
      k_ok = 0;
      break;
    }
    else if (k_back == 1)break;
  }
  lcd.clear();
  lcd.print("Calibrat DONE!");
  delay(1000);
  lcd.clear();
}

void Calibrat_ServoMP(void)
{
  int xmp = posx_mp, ymp = posy_mp;
  unsigned long send_time;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Adjust Servo");
  lcd.setCursor(1, 1);
  lcd.print("and Press OK");
  delay(1000);
  lcd.clear();

  while (1)
  {
    KEY_Read();
    if (k_down && ymp < 170)k_down = 0, ymp++;
    if (k_up && ymp > 60)k_up = 0, ymp--;
    if (k_r == 1 && xmp > 10)k_l == 0, xmp--;
    if (k_l == 1 && xmp < 170)k_r == 0, xmp++;

    lcd.setCursor(1, 0);
    lcd.print("Servo:");
    lcd.setCursor(1, 1);
    lcd.print("X=");
    lcd.print(xmp);
    lcd.print("  ");
    lcd.setCursor(9, 1);
    lcd.print("Y=");
    lcd.print(ymp);
    lcd.print("  ");

    Exchg();
    if (millis() >= send_time)SEND(), send_time = millis() + S_freq;
    if (k_ok == 1)
    {
      posx_mp = xmp;
      posy_mp = ymp;
      k_ok = 0;
      break;
    }
    else if (k_back == 1)break;
  }
  lcd.clear();
  lcd.print("Calibrat DONE!");
  delay(1000);
  lcd.clear();
}
