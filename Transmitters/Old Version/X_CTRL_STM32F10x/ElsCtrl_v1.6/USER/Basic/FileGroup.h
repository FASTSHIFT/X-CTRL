#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************** STM32 Core *****************//
#include "math.h"
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "HardwareSerial.h"
#include "SPI.h"
#include "Wire.h"
#include "WString.h"
#include "Stream.h"
#include "WMath.h"
#include "Tone.h"

//************* User Libraries ************//
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"
#include "ButtonEvent.h"
#include "filters.h"

//**************Default Config**************//
#define ADC_MaxValue 4095
#define TIM_ControlThread TIM2
#define TIM_DisplayThread TIM3
#define NRF_AddressNum 6

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

typedef struct {
    uint8_t Address;
    uint8_t Speed;
    uint8_t Freq;
} NRF_Config_TypeDef;

//************* Object & Struct & Value**************//
/*Object*/
extern ButtonEvent btUP;
extern ButtonEvent btDOWN;
extern ButtonEvent btOK;

/*Struct*/
extern Protocol_Common_t CTRL;
extern Joystick_t JS_L;
extern NRF_Config_TypeDef NRF_Cfg;

/*Value*/
extern bool State_RF;
extern bool State_PassBack;
extern bool State_Handshake;
extern bool State_BuzzSound;
extern bool State_MusicPlayer;
extern float BattUsage;
extern float BattVoltage;
extern float ElsSpeed;
extern float ElsDistance;
extern uint8_t NRF_AddressConfig[NRF_AddressNum * 5];
extern int16_t NRF_SignalStrength;

//*************GPIO**************//

/*ADC*/
#define JSL_X_Pin        PA1
#define JSL_Y_Pin        PA2
#define BattSensor_Pin   PB1
#define JSL_X_ADC        Get_DMA_ADC(0)
#define JSL_Y_ADC        Get_DMA_ADC(1)
#define BattSensor_ADC   Get_DMA_ADC(2)

/*Other*/
#define ChargeDetect_Pin PB0
#define Buzz_Pin         PA11
#define InfoLED_Pin      PA9

/*Button*/
#define Button_UP_Pin    PA6
#define Button_DOWN_Pin  PA5
#define Button_OK_Pin    PA7

/*NRF*/
#define NRF_MOSI_Pin     PB4
#define NRF_MISO_Pin     PB3
#define NRF_SCK_Pin      PB5
#define NRF_CE_Pin       PB7
#define NRF_CSN_Pin      PB6
#define NRF_IRQ_Pin      PA15
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

//****************** Functions********************//
void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);

bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Thread_GUI();

void Init_ElsCtrl();
bool Init_Value();
void Init_Display();
bool Init_NRF();
void Init_BottonEvent();
void Init_GUI();

void Task_SensorUpdate();
void Task_TransferData();
void Task_BuzzSignalMonitor();
void Task_ScreenDisplay();
void Task_MusicPlayerRunning();

void NRF_IRQHandler();

#endif
