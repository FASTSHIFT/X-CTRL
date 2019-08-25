#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************** Core *****************//
#include "stm32f10x.h"
#include "Arduino.h"
#include "math.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "time_exti.h"
#include "C_call_CPP.h"

//************* Arduino API **************//
#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"
#include "WString.h"
#include "Stream.h"

//************* User Libraries ************//
#include "extEEPROM.h"
#include "NRF.h"
#include "SSD1306_I2C.h"
#include "Picture.h"
#include "MPU6050.h"

//**************Default Config**************//
#define _X_CTRL_VERSION "V2.1"
#define SendFreq_Default 10//ms
#define DisplayFreq_Default 30000//us
#define CtrlMode_Default 1
#define CtrlObject_Default CAR
#define UART_Baud_Select_Default 10//115200
#define ADC_MaxValue 4095
#define PWM_MaxValue 255
#define OLED_Display(x) 	 TIM_Cmd(TIM4,x)
#define SendData_Switch(x) TIM_Cmd(TIM2,x)
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.2
#define MPU_Pitch_Attach_Default (&(JS_R.Y))
#define MPU_Roll_Attach_Default (&(JS_R.X))
#define MPU_ReadFreq_Default 10//ms
#define MPU_Max_Default 12000

//*************Object**************//
extern SSD1306_I2C oled;
extern NRF nrf;
extern MPU6050 mpu;

//*************GPIO**************//
#define JSL_X_Pin PA0
#define JSL_Y_Pin PA1
#define JSR_X_Pin PA2
#define JSR_Y_Pin PA3
#define ADL_Pin PA4
#define ADR_Pin PA5
#define BattSensor_Pin PA6

#define EncoderLED_Pin PB1
#define EncoderKey_Pin PB9
#define EncoderA_Pin PB8
#define EncoderB_Pin PB7

#define BUZZ_Pin PB12
#define SPDT_Switch_Pin PA15

#define HC165_OUT_Pin		PB4
#define HC165_CP_Pin		PB5
#define HC165_PL_Pin		PB6 

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin PB13
#define NRF_CE_Pin PA12
#define NRF_CSN_Pin PA11
#define NRF_IRQ_Pin
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin PC13
#define BT_State_Pin PC14
#define BT_EN_Pin PC15

//*************Enum**************//
typedef enum{
	CtrlPattern_UART,
	CtrlPattern_NRF
}CtrlPattern_TypeDef;

typedef enum{
	CAR,
	TANK,
	CPTR,
	SPIDER,
	CRAWLER
}Ctrl_Object_TypeDef;
#define IS_USE_ORIGIN(x) ((x == CPTR)||(x == SPIDER))

typedef enum{
	INF,
	OPT
}MenuState_TypeDef;

//*************Values**************//

typedef struct CtrlOption{
	uint16_t SendFreq;
	uint8_t Baud_Select;
	uint16_t Mode;
	uint16_t Pattern;
	uint8_t Object;
}CtrlOption_TypeDef;

typedef struct DataPackage{
	uint16_t JS_LX;
	uint16_t JS_LY;
	uint16_t JS_RX;
	uint16_t JS_RY;
	uint16_t Rotoary_L;
	uint16_t Rotoary_R;
	uint8_t KeyValue;
	uint8_t NowCtrlObject;
}DataPackage_TypeDef;

extern CtrlOption_TypeDef CTRL;
extern const uint32_t UART_Baud[];

typedef struct CtrlParamter
{
  int X, Xmin, Xmp, Xmax;
  int Y, Ymin, Ymp, Ymax;
}CtrlParamter_TypeDef;

extern CtrlParamter_TypeDef JS_L;
extern CtrlParamter_TypeDef JS_R;
extern CtrlParamter_TypeDef ServoPos;

extern int L_PWM, R_PWM, B_PWM;
extern int L_CRE, R_CRE;

extern uint8_t Triangle[];
extern uint8_t Triangle_UD[];
extern uint8_t Arrow_UpDown[];
extern uint8_t Arrow_Left[];
extern uint8_t Arrow_Right[];

extern uint8_t SPDT_State;
extern uint8_t Key[8];
typedef enum{
	k_up,
	k_down,
	k_ok,
	k_back,
	k_jsl,
	k_jsr,
	k_l,
	k_r
}Key_TypeDef;

extern uint32_t Recv_TimePoint;
extern uint32_t Tran_TimePoint;
extern uint8_t TX_buff[14];
extern uint8_t RX_buff[14];
extern const uint8_t RX_DataLength;

extern uint8_t CTRL_State;
extern uint8_t OLED_Brightness;
extern uint8_t Ring_Last;
extern uint8_t Silent_Mode;
extern uint8_t MenuState;
extern uint8_t MPU_Switch;
extern uint8_t MPU_Reading;
extern uint16_t MPU_ReadFreq;
extern int16_t MPU_Pitch, MPU_Roll, MPU_Null, MPU_Max;
extern int* MPU_Pitch_Attach;
extern int* MPU_Roll_Attach;
extern uint32_t MainLoop_Speed;

//****SlaveInfo****//
extern float Slave_Temp, Slave_BattVol;
extern float CR_kpa, CR_alt, CR_roll, CR_pitch, CR_FPV_Vol;
extern float Car_Kph, Car_RPS;
//***************//

//***************************File/Functoins*******************************//
//BlueToothSetup.cpp
void BT_Setup(void);

//BuzzRing.cpp
void BuzzRing(uint8_t chs);

//Calibration.cpp
void Calibrat_JSMP();
void Calibrat_JSMaxMin();
void Calibrat_ServoMP(int XmpMin, int XmpMax, int YmpMin, int YmpMax);
void Calibrat_MPU6050();
void Init_NRF();

//DataProcess.cpp
void TX_DataLoad();
void SendData();
void RX_DataLoad();
void RX_Transform();
void NRF_CheckConnection();
uint8_t Get_ConnectionState();

//DataStorage.cpp
typedef enum {
	ReadData, 
	SaveData, 
	ClearData
} DataStorageHandle_TypeDef;
void DataStorageHandle(DataStorageHandle_TypeDef Select);
void CTRL_SetDefault();

//Display.cpp
void InfoDisplay();
void OLED_Refresh_Handler();
void OLED_DynamicPrint(uint16_t x, uint16_t y, String Str, uint32_t dly);
void AboutInfoPage();

//HMI_Display.cpp
void HMI_Display();

//Joystick_Key_Read.cpp
void Pin_Config();
void Read_Joystick();
void Read_Encoder();
void Read_Keys();
void KeyClear();
void EncoderLED_Handler();
uint8_t CompressToByte(uint8_t* Array);
void Read_MPU6050(uint32_t Time_ms);
float Read_BattVoltage();

//LoadCtrlMode.cpp
void LoadCtrlMode(uint8_t cmd);

//Menu.cpp
void MenuRun(void);
void NumClear(unsigned long Num,int digit);

//Tools.cpp
void IIC_Scan();
void Limit_int16(int16_t *data, int16_t MIN, int16_t MAX);
void LoopExecution(void_func_point Func, uint32_t Times);
void LoopSpeedDetect();

#endif
