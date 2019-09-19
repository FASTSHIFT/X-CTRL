#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

/** 
  *@Illustrate: A Cross-platform migration Project | Arduino for STM32F10x 
  *@Author: FASTSHIFT
  *@Website: https://github.com/FASTSHIFT/Arduino-For-Keil
  */

//************* Arduino Library **************//
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Stream.h"
#include "SPI.h"
#include "WString.h"
#include "Wire.h"
#include "Tone.h"
#include "WMath.h"

//************* STM32 Core Library **************//
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"

//************* USER Library **************//
#include "Adafruit_GFX.h"
#include "FreeSans9pt7b.h"
#include "Adafruit_ST7735.h"
#include "Adafruit_AMG88xx.h"
#include "interpolation.h"
#include "ButtonEvent.h"
#include "MillisTaskManager.h"
#include "NRF.h"
#include "CommonMacro.h"

//************* Define **************//
#define TFT_DC PA3
#define TFT_CS PA2
#define TFT_MOSI PA7
#define TFT_CLK PA5
#define TFT_RST PA4
#define TFT_BLK PA1

#define InfoLED_Pin PC13
#define Button_UP_Pin PB11
#define Button_Down_Pin PB10

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin PB13
#define NRF_CE_Pin PA8
#define NRF_CSN_Pin PB12
#define NRF_IRQ_Pin -1
/**NRF24L01 引脚分布图(模块正面,俯视)
  *----------------+-------+
  *|               |       |
  *|        _______|_______|
  *|        |8:IRQ | 7:MISO|
  *|        |6:MOSI| 5:SCK |
  *|        |4:CSN | 3:CE  |
  *|        |2:VCC | 1:GND |
  *----------------+-------+
  */

#define AMG_COLS 8
#define AMG_ROWS 8
//#define INTERPOLATED_COLS 40
//#define INTERPOLATED_ROWS 40
#define ResolutionRatio_SelectDefault 5

//Comment this out to remove the text overlay
#define SHOW_TEMP_TEXT

//low range of the sensor (this will be blue on the screen)
#define MINTEMP_DEFAULT 0

//high range of the sensor (this will be red on the screen)
#define MAXTEMP_DEFAULT 30

#define SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define LimitValue(x,min,max) x=constrain(x,min,max)

//************** Extern ******************//
extern Adafruit_AMG88xx amg;
extern Adafruit_ST7735 tft;

extern float AMG_OriginPixels[AMG_COLS * AMG_ROWS];
extern uint8_t MinTemp;
extern uint8_t MaxTemp;
extern uint8_t ResolutionRatio[6];
extern uint8_t ResolutionRatio_Select;

//**************** Functions ****************//
void AMG_Drawpixels(float *p, uint8_t rows, uint8_t cols, uint8_t boxWidth, uint8_t boxHeight, bool showVal);
void Init_ButtonEvent();
bool Init_NRF();
void Task_ButtonEventMonitor();
void Task_NRF_DataTran();
void NRF_SendPixels(uint8_t *buf, uint16_t size);

#endif
