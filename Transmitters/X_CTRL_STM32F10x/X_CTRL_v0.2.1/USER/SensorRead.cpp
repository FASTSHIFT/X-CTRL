#include "FileGroup.h"

MPU6050 mpu;
uint8_t MPU_Switch = Off;
uint16_t MPU_ReadFreq = MPU_ReadFreq_Default;
int16_t MPU_Pitch, MPU_Roll, MPU_Null, MPU_Max = MPU_Max_Default;
int* MPU_Pitch_Attach =&(JS_R.Y);
int* MPU_Roll_Attach = &(JS_R.X);

CtrlParamter_TypeDef JS_L;
CtrlParamter_TypeDef JS_R;
CtrlParamter_TypeDef ServoPos;

uint8_t Key[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t SPDT_State = 0;

void Pin_Config()
{	
//	pinMode(JSL_X_Pin, INPUT_ANALOG);
//	pinMode(JSL_Y_Pin, INPUT_ANALOG);
//	pinMode(JSR_X_Pin, INPUT_ANALOG);
//	pinMode(JSR_Y_Pin, INPUT_ANALOG);
//	pinMode(ADL_Pin, INPUT_ANALOG);
//	pinMode(ADR_Pin, INPUT_ANALOG);
//	pinMode(BattSensor_Pin, INPUT_ANALOG);
//	Use DMA
	
	pinMode(EncoderLED_Pin, PWM);
	pinMode(EncoderKey_Pin, INPUT_PULLUP);
	pinMode(EncoderA_Pin, INPUT);
	pinMode(EncoderB_Pin, INPUT);
		
	pinMode(BUZZ_Pin, OUTPUT);
	pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);
	
	pinMode(HC165_OUT_Pin, INPUT);
	pinMode(HC165_CP_Pin, OUTPUT);
	pinMode(HC165_PL_Pin, OUTPUT);
	
	pinMode(BT_PowerCtrl_Pin, OUTPUT);
	pinMode(BT_State_Pin, INPUT);
	pinMode(BT_EN_Pin, OUTPUT);
}

int JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX)
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

void Read_Joystick()
{
  JS_L.Y = JoystickMap(analogRead_DMA(JSL_Y_Pin), JS_L.Ymin, JS_L.Ymp, JS_L.Ymax);
  JS_R.Y = JoystickMap(analogRead_DMA(JSR_Y_Pin), JS_R.Ymin, JS_R.Ymp, JS_R.Ymax);
  JS_L.X = JoystickMap(analogRead_DMA(JSL_X_Pin), JS_L.Xmin, JS_L.Xmp, JS_L.Xmax);
  JS_R.X = JoystickMap(analogRead_DMA(JSR_X_Pin), JS_R.Xmin, JS_R.Xmp, JS_R.Xmax);

  L_CRE = analogRead_DMA(ADL_Pin);
  R_CRE = analogRead_DMA(ADR_Pin);
}

void EncoderLED_Handler()
{
	static uint16_t Bright, Bright_Set;
	static uint32_t TimePoint;
	static uint8_t EncoderState_Last;
	if(digitalRead(EncoderKey_Pin) == 0 || digitalRead(EncoderA_Pin) != EncoderState_Last)
	{
		Bright_Set = 300;
		TimePoint = millis() + 500;
	}
	else if(millis() > TimePoint)
	{
		Bright_Set = 0;
	}
	
	if(Bright > Bright_Set)Bright-=10;
	else if(Bright < Bright_Set)Bright+=20;
	
	EncoderState_Last = digitalRead(EncoderA_Pin);
	
	pwmWrite(EncoderLED_Pin, Bright);
}

void Read_Encoder()
{
  static boolean encoderALast = LOW;
  boolean encoderA = digitalRead(EncoderA_Pin);
  if ((encoderALast == HIGH) && (encoderA == LOW))
  {
    if (digitalRead(EncoderB_Pin) == LOW)
    {
      Key[k_up] = 1;
    }
    else
    {
      Key[k_down] = 1;
    }
  }
  encoderALast = encoderA;

	static uint8_t KeyState_Last;
	static uint32_t PressKey_TimePoint;
	uint8_t KeyState = !digitalRead(EncoderKey_Pin);
  
	if(KeyState == 1 && KeyState_Last == 0)//Press key ,mark TimePoint
	{
		PressKey_TimePoint = millis() + 500;
	}
	else if(KeyState == 0 && KeyState_Last == 1)//Release key,check TimePoint
	{
		if(millis() < PressKey_TimePoint)//Short press
		{
			Key[k_ok] = 1;
		}
		else//Long Press
		{
			Key[k_back] = 1;
		}
	}
	KeyState_Last = KeyState;//Backup key state
}

uint8_t Get_HC165_Value()
{
	digitalWrite_HIGH(HC165_CP_Pin);       
	digitalWrite_LOW(HC165_PL_Pin);
	digitalWrite_HIGH(HC165_PL_Pin);
	return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

void Read_Keys()
{
	if(MenuState == OPT)KeyClear();
  uint8_t BottonValue = Get_HC165_Value();
	
	static uint32_t PressKey_TimePoint;
  static bool IS_Pressed = false;

  if (!IS_Pressed && BottonValue)
  {
		if (Silent_Mode == Off && MenuState == OPT)BuzzRing(5);
    if (bitRead(BottonValue,4))Key[k_up] = 1;
    if (bitRead(BottonValue,7))Key[k_down] = 1;
    if (bitRead(BottonValue,6))Key[k_ok] = 1;
    if (bitRead(BottonValue,5))Key[k_back] = 1;
    if (bitRead(BottonValue,1))Key[k_jsl] = 1;
    if (bitRead(BottonValue,2))Key[k_jsr] = 1;
    if (bitRead(BottonValue,0))Key[k_l] = 1;
    if (bitRead(BottonValue,3))Key[k_r] = 1;
    IS_Pressed = true;
    PressKey_TimePoint = millis() + 500;
  }
  else if (IS_Pressed && millis() >= PressKey_TimePoint && BottonValue)
  {
    if (Silent_Mode == Off && MenuState == OPT)BuzzRing(5);
    if (bitRead(BottonValue,4))Key[k_up] = 1;
    if (bitRead(BottonValue,7))Key[k_down] = 1;
    if (bitRead(BottonValue,6))Key[k_ok] = 1;
    if (bitRead(BottonValue,5))Key[k_back] = 1;
    if (bitRead(BottonValue,1))Key[k_jsl] = 1;
    if (bitRead(BottonValue,2))Key[k_jsr] = 1;
    if (bitRead(BottonValue,0))Key[k_l] = 1;
    if (bitRead(BottonValue,3))Key[k_r] = 1;
  }
  else if (IS_Pressed && !BottonValue)
  {
		KeyClear();
		IS_Pressed = false;
  }
	Read_Encoder();
	SPDT_State = digitalRead(SPDT_Switch_Pin);
}

void KeyClear()
{
	Key[k_up] = 0;
	Key[k_down] = 0;
	Key[k_ok] = 0;
	Key[k_back] = 0;
	Key[k_jsl] = 0;
	Key[k_jsr] = 0;
	Key[k_l] = 0;
	Key[k_r] = 0;
}

uint8_t CompressToByte(uint8_t* Array)
{
	return((Array[0] << 7)| 
				 (Array[1] << 6)|
				 (Array[2] << 5)|
				 (Array[3] << 4)|
				 (Array[4] << 3)| 
				 (Array[5] << 2)| 
				 (Array[6] << 1)| 
				  Array[7]);
}

void Read_MPU6050(uint32_t Time_ms)
{
	static uint32_t TimePoint;
	if(millis() > TimePoint)
	{
		mpu.getMotion6(&MPU_Roll, &MPU_Pitch, &MPU_Null, &MPU_Null, &MPU_Null, &MPU_Null);
		Limit_int16(&MPU_Roll, -MPU_Max, MPU_Max);
		Limit_int16(&MPU_Pitch, -MPU_Max, MPU_Max);
		TimePoint = millis() + Time_ms;
	}
}

float Read_BattVoltage()
{
	return ((analogRead_DMA(BattSensor_Pin)/float(ADC_MaxValue)) * 3.3 * 2.0);
}
