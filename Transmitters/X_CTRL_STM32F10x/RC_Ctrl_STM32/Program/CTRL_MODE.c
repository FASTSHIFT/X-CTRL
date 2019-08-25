#include "FileGroup.h"

void Ctrl_Mode(unsigned char cmd)
{
  float l_cre = L_CRE / 1023.0;
  float r_cre = R_CRE / 1023.0;

  switch (cmd)
  {
    case 1:
      L_PWM = L_Y * l_cre, R_PWM = R_Y * r_cre;
      break;

    case 2:
      if (L_Y >= 0) L_PWM = L_Y * l_cre + R_X * r_cre, R_PWM = L_Y * l_cre - R_X * r_cre;
      else L_PWM = L_Y * l_cre - R_X * r_cre, R_PWM = L_Y * l_cre + R_X * r_cre;
      break;

    case 3:
      if (L_Y >= 0) L_PWM = L_Y * l_cre + L_X * l_cre * 0.5, R_PWM = L_Y * l_cre - L_X * l_cre * 0.5;
      else L_PWM = L_Y * l_cre - L_X * l_cre * 0.5, R_PWM = L_Y * l_cre + L_X * l_cre * 0.5;

      if (R_X >= 0) posx = 180 - map(R_X, 0, 255, 180 - posx_mp, 180);
      else if (R_X < 0) posx = 180 - map(R_X, -255, 0, 0, 180 - posx_mp);
      if (R_Y >= 0)posy = 180 - map(R_Y, 0, 255, 180 - posy_mp, 130);
      else if (R_Y < 0)posy = 180 - map(R_Y, -255, 0, 0, 180 - posy_mp);

      B_PWM = -R_X;
      break;
  }
  if (cmd != 3 && Menu_Sta == INF && CTRL_object < CPTR)
  {
    if (CTRL_module == UART)
    {
      if (k_down)k_down = 0, posy++;
      if (k_up)k_up = 0, posy--;
      if (k_r)
      {
        k_l = 0;
        posx--;
        B_PWM--;
      }
      if (k_l)
      {
        k_r = 0;
        posx++;
        B_PWM++;
      }
      if (k_r + k_l == 0)B_PWM = 0;
    }
    else
    {
      if (k_jsr)k_jsl = 0, posy++;
      if (k_jsl)k_jsr = 0, posy--;
      if (k_r)k_l = 0, posx--;
      if (k_l)k_r = 0, posx++;
    }
    if (k_ok)
    {
      k_ok = 0;
      posx = posx_mp;
      posy = posy_mp;
    }
  }

  if (CTRL_object == CPTR && Menu_Sta == INF)
  {
    /*if (MPU_ON)
      {
      //if (fLastRoll < 3.0 && fLastRoll > -3.0)fLastRoll = 0.0;
      //if (fLastPitch < 3.0 && fLastPitch > -3.0)fLastPitch = 0.0;

      TX_buff[3] = float(fLastPitch / 70.0) * r_cre * 100.0 + 100.0;
      TX_buff[4] = float(fLastRoll / 70.0) * r_cre * 100.0 + 100.0;
      }*/
  }

  Limit(&L_PWM, -255, 255);
  Limit(&R_PWM, -255, 255);
  Limit(&B_PWM, -255, 255);
  Limit(&posx, 0, 180);
  Limit(&posy, 50, 180);
}
