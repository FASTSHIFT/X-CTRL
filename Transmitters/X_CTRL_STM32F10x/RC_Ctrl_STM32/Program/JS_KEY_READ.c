#include "FileGroup.h"

extern int L_PWM, L_X, L_Y, L_XMP, L_YMP, L_XMAX, L_XMIN, L_YMAX, L_YMIN;
extern int R_PWM, R_X, R_Y, R_XMP, R_YMP, R_XMAX, R_XMIN, R_YMAX, R_YMIN;
extern int L_CRE, R_CRE;
extern u8 k_up, k_down, k_l, k_ok, k_r;


#define sst 50
#define BT_DOWN k_read>50
int K_TV[5]={65535*(1/5),65535*(2/5),65535*(3/5),65535*(4/5),65535};

int Read_JS(int aR, int MIN, int MP, int MAX)
{
  int dat;
  if ((aR - MP) >= 0)dat = map(aR, MP, MAX, 0, 255);
  else dat = map(aR, MIN, MP, -255, 0);
  return dat;
}

void JS_Read(void)
{
  L_Y = Read_JS(analogRead(JSL_Y), L_YMIN, L_YMP, L_YMAX);
  R_Y = Read_JS(analogRead(JSR_Y), R_YMIN, R_YMP, R_YMAX);
  L_X = Read_JS(analogRead(JSL_X), L_XMIN, L_XMP, L_XMAX);
  R_X = Read_JS(analogRead(JSR_X), R_XMIN, R_XMP, R_XMAX);

  L_CRE = analogRead(ADL);
  R_CRE = analogRead(ADR);
}

void KEY_Read(void)
{
  static unsigned long KEY_time;
  static u8 press_flag = 0;
	int k_read = analogRead(KEYS);

  if (press_flag == 0 && (BT_DOWN != 0))
  {
		if ((k_read>K_TV[0]-sst) && (k_read>K_TV[0]+sst))k_up = 1;
		else if ((k_read>K_TV[1]-sst) && (k_read>K_TV[1]+sst))k_down = 1;
		else if ((k_read>K_TV[2]-sst) && (k_read>K_TV[2]+sst))k_ok = 1;
		else if ((k_read>K_TV[3]-sst) && (k_read>K_TV[3]+sst))k_l = 1;
		else if ((k_read>K_TV[4]-sst) && (k_read>K_TV[4]+sst))k_r = 1;
		press_flag = 1;
    KEY_time = millis() + 500;
  }
  else if (press_flag == 1 && millis() >= KEY_time && (BT_DOWN != 0))
  {
    if ((k_read>K_TV[0]-sst) && (k_read>K_TV[0]+sst))k_up = 1;
		else if ((k_read>K_TV[1]-sst) && (k_read>K_TV[1]+sst))k_down = 1;
		else if ((k_read>K_TV[2]-sst) && (k_read>K_TV[2]+sst))k_ok = 1;
		else if ((k_read>K_TV[3]-sst) && (k_read>K_TV[3]+sst))k_l = 1;
		else if ((k_read>K_TV[4]-sst) && (k_read>K_TV[4]+sst))k_r = 1;
  }

  else if (press_flag == 1 && (BT_DOWN == 0))
  {
		k_up = 0;
		k_down = 0;
		k_ok = 0;
		k_l = 0;
		k_r = 0;

		press_flag = 0;
  }
}
