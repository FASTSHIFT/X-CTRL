#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************C/C++ Library**************//

#include "Arduino.h"
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"
#include "math.h"
#include "WMath.h"
#include "Tone.h"

#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"
#include "WString.h"
#include "Stream.h"

#include "C_call_CPP.h"

//************* User Libraries ************//
#include "RC_Protocol.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"
#include "Adafruit_ST7735.h"
#include "Picture.c"
#include "MillisTaskManager.h"
#include "NRF.h"
#include "MPU6050.h"
#include "ButtonEvent.h"
#include "PageManager.h"
#include "Bitmap.h"

extern Adafruit_ST7735 tft;
extern NRF nrf;
extern MPU6050 mpu;
extern PageManager pm;

extern ButtonEvent btUP;
extern ButtonEvent btDOWN;
extern ButtonEvent btLEFT;
extern ButtonEvent btRIGHT;
extern ButtonEvent btA;
extern ButtonEvent btB;
extern ButtonEvent btL1;
extern ButtonEvent btR1;

//**************Default Config**************//
#define _TriggerCtrl_VERSION "V3.0"
#define TaskManager_Timer TIM2
#define TaskManager_Freq 1000//us
#define SendFreq_Default 10//ms
#define CtrlMode_Default CtrlMode_Race
#define CtrlObject_Default X_COMMON
#define UART_Baud_Select_Default 10//115200
#define ADC_MaxValue 4095
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.2
#define MPU_ReadFreq_Default 10//ms
#define MPU_Max_Default 12000

#define BlueTooth Serial
#define HMI Serial
#define LimitValue(x,min,max) (x=constrain(x,min,max))

#define ConnectLost_TimeOut 500

#define NRF_AddressConfig 75,75,75,75,75

#define clear() fillScreen(ST77XX_BLACK)

typedef enum{
	PAGE_MainMenu,
	PAGE_CtrlInfo,
	PAGE_Option,
	PAGE_END
}PageNum_TypeDef;

typedef struct{
	float Channel_1;
	float Channel_2;
	float Channel_3;
	float Channel_4;
	float Channel_5;
	float Channel_6;
	float Channel_7;
	float Channel_8;
}Slave_DataPack_t;

//*************** GPIO Config ***************//
#define TFT_CS     PA8
#define TFT_RST    PB9
#define TFT_DC     PB8

#define JSL_X_Pin PA2
#define JSL_Y_Pin PA3
#define JSR_X_Pin PA0
#define JSR_Y_Pin PA1
#define ADR_Pin PB1
#define ADL_Pin PB0
#define BattSensor_Pin PA4

#define HC165_OUT_Pin PC14
#define HC165_CP_Pin PC13
#define HC165_PL_Pin PC15

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin PB13
#define NRF_CE_Pin PB12
#define NRF_CSN_Pin PB11
#define NRF_IRQ_Pin -1
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define SW_L_Pin PA12
#define SW_R_Pin PA11

#define Buzz_Pin PA15

#define SL digitalRead_FAST(SW_L_Pin)
#define SR digitalRead_FAST(SW_R_Pin)
//************* Extern Value ************//
extern Protocol_CarDS_t CarDS;
extern Protocol_CarSS_t CarSS;
extern Protocol_Common_t CTRL;
extern Slave_DataPack_t CarSS_Slave;
extern ConnectState_t ConnectState;
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern Axis_t MPU_Data;
extern float BattVoltage;
extern bool RF_State;
extern bool PassBack_State;

//**************** Functions *********************//
void Init_TriggerCtrl();
void Init_NRF();
void Init_ButtonEvent();
void ButtonEvent_Monitor();

//**************** Tasks *********************//
void Task_SensorUpdate();
void Task_LoadCtrlMode();
void Task_TransferData();
void Task_MPU6050Read();

void PageManager_Running();
//tone//
void nokia_tune(int volume,float speed);
//display//
void Boot_logo();
void Boot_logo2();

void PageRegister_MainMenu();
void PageRegister_CtrlInfo();
void PageRegister_Option();
#endif
