#include "FileGroup.h"

int L_PWM, R_PWM, B_PWM;
int L_CRE, R_CRE;

void LoadCtrlMode(unsigned char cmd)
{
  float Limit_L = L_CRE / float(ADC_MaxValue);
  float Limit_R = R_CRE / float(ADC_MaxValue);
	
	if (MPU_Switch == On && MenuState == INF)
  {
		*MPU_Roll_Attach = map(MPU_Roll, -MPU_Max, MPU_Max, -PWM_MaxValue, PWM_MaxValue);
		*MPU_Pitch_Attach = map(MPU_Pitch, -MPU_Max, MPU_Max, -PWM_MaxValue, PWM_MaxValue);
  }

  switch (cmd)
  {
    case 1:
      L_PWM = JS_L.Y * Limit_L;
			R_PWM = JS_R.Y * Limit_R;
      break;

    case 2:
      if (JS_L.Y >= 0)
      {
        L_PWM = JS_L.Y * Limit_L + JS_R.X * Limit_R;
        R_PWM = JS_L.Y * Limit_L - JS_R.X * Limit_R;
      }
      else
      {
        L_PWM = JS_L.Y * Limit_L - JS_R.X * Limit_R;
        R_PWM = JS_L.Y * Limit_L + JS_R.X * Limit_R;
      }
      break;

    case 3:
      if (JS_L.Y >= 0)
      {
        L_PWM = JS_L.Y * Limit_L + JS_L.X * Limit_L * 0.5;
        R_PWM = JS_L.Y * Limit_L - JS_L.X * Limit_L * 0.5;
      }
      else
      {
        L_PWM = JS_L.Y * Limit_L - JS_L.X * Limit_L * 0.5;
        R_PWM = JS_L.Y * Limit_L + JS_L.X * Limit_L * 0.5;
      }

      if (JS_R.X >= 0) ServoPos.X = 180 - map(JS_R.X, 0, 255, 180 - ServoPos.Xmp, 180);
      else if (JS_R.X < 0) ServoPos.X = 180 - map(JS_R.X, -255, 0, 0, 180 - ServoPos.Xmp);
      if (JS_R.Y >= 0)ServoPos.Y = 180 - map(JS_R.Y, 0, 255, 180 - ServoPos.Ymp, 180);
      else if (JS_R.Y < 0)ServoPos.Y = 180 - map(JS_R.Y, -255, 0, 0, 180 - ServoPos.Ymp);

      B_PWM = -JS_R.X;
      break;
  }
  if (cmd != 3 && MenuState == INF && (CTRL.Object == CAR || CTRL.Object == TANK || CTRL.Object == CRAWLER))
  {
    static unsigned long PosAdd_Time;
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

	L_PWM = constrain(L_PWM,-255,255);
	R_PWM = constrain(R_PWM,-255,255);
	B_PWM = constrain(B_PWM,-255,255);
	ServoPos.X = constrain(ServoPos.X,0,180);
	ServoPos.Y = constrain(ServoPos.Y,0,180);
}

void LoopExecute()
{	
	if (CTRL.Pattern == CtrlPattern_UART && BlueTooth.available() >= RX_DataLength)
	{
		RX_DataLoad();
	}
  else //CTRL.Pattern == CtrlPattern_NRF
  {
    NRF_CheckConnection();
    RX_Transform();
  }
	
	Read_Joystick();
	Read_Keys();
	if(MPU_Switch == On) Read_MPU6050(MPU_ReadFreq);
	
  if(MenuState == INF)
	{
		LoadCtrlMode(CTRL.Mode);	
		TX_DataLoad();
	}
}
