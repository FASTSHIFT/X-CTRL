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

//**************************** USER **********************************//
#include <LiquidCrystal_I2C.h>
#include <SSD1306_I2C.h>
#include <NRF.h>

#define MsTimer2_set(time,func) TimerSet(TIM2,time*1000,func)
#define MsTimer2_start() TIM_Cmd(TIM2,ENABLE)
#define MsTimer2_stop()  TIM_Cmd(TIM2,DISABLE)

#define USE_MPU6050
#define _VERSION "V4.1"
#define SendFreq_Default 10//ms
#define CtrlMode_Default 3
#define UART_Baud_Select_Default 10//115200
#define ADC_Bits 12
#define ADC_MaxValue 4095//(pow(2, ADC_Bits) - 1)
#define PWM_MaxValue 255

#ifdef USE_MPU6050
//********************MPU6050**************************//
#include <MPU6050.h>
extern MPU6050 mpu;
extern int16_t MPU_Pitch, MPU_Roll, MPU_Null;
#define MPU_MAX 12000
//**************************************************//
#endif

extern SSD1306_I2C oled;
extern LiquidCrystal_I2C lcd;//0x3F
extern NRF nrf; //MOSI,MISO,SCK,CE,CSN
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

typedef struct CtrlParamter
{
  int X, Xmin, Xmp, Xmax;
  int Y, Ymin, Ymp, Ymax;
}CtrlParamter_t;

extern CtrlParamter_t JS_L;
extern CtrlParamter_t JS_R;
extern CtrlParamter_t ServoPos;

extern int L_PWM, R_PWM, B_PWM;
extern int L_CRE, R_CRE;

extern const uint8_t RX_DataLength;
extern const uint8_t TX_DataLength;
extern uint8_t TX_buff[14];
extern uint8_t RX_buff[14];
extern uint8_t RX_DataPackage_Num;

//************* GPIO **************//
//ADC
#define JSL_X_Pin A7
#define JSL_Y_Pin A6
#define JSR_X_Pin A3
#define JSR_Y_Pin A2
#define ADL_Pin   A1
#define ADR_Pin   A0

//Others
extern uint8_t KEY_UP_Pin; //D12
extern uint8_t KEY_BACK_Pin; //D11
#define KEY_OK_Pin D10
extern uint8_t KEY_DOWN_Pin; //D9
#define KEY_JSR_Pin D8
#define KEY_JSL_Pin D7
extern uint8_t KEY_R_Pin; //D6
extern uint8_t KEY_L_Pin; //D5

#define BUZZ_Pin D3

//*************Value**************//

//******从机******//
extern float Slave_Temp, Slave_BattVol;
extern float CR_kpa, CR_alt, CR_roll, CR_pitch, CR_FPV_Vol;
extern float Car_Kph, Car_RPS;
//***************//

extern uint8_t Key[8];
typedef enum {k_up, k_down, k_ok, k_back, k_jsl, k_jsr, k_l, k_r} Key_Type;

extern uint8_t CTRL_State;//启动控制

typedef enum {Module_UART, Module_NRF} CTRL_Module_Type;
extern uint8_t CTRL_Module;//模块选择 0:UART 1:NRF24L01

typedef enum {ReadData, SaveData, ClearData} DataStorageHandle_Type;//数据存储操作

typedef enum {CAR, TANK, CPTR, SPIDER, CRAWLER} CtrlObject_Type;
extern int CTRL_Object;//操纵对象

typedef enum {INF, OPT} MenuState_Type;
extern uint8_t MenuState;//0:显示信息 1:进入设置菜单

extern const uint32_t UART_Baud[]; //常用波特率
extern uint8_t UART_Baud_Select;//通信波特率选择

extern uint32_t Recv_TimePoint;//接收时间点
extern uint32_t Tran_TimePoint;//发送时间点
extern uint16_t SendFreq;//数据包发送频率
extern uint8_t CtrlMode;//操纵模式 1:左右独立控制 2:左油门，右转向 3:左油门转向，右舵机

extern uint8_t MPU_Switch;//加速度计
extern uint8_t BackLight_Switch;//背光
extern uint8_t RingLast;
extern uint8_t Silent_Mode;

//********************* LCD自定义字符 ********************//
extern uint8_t ANT[];
extern uint8_t ANT_full[];
extern uint8_t Tempdc[];
extern uint8_t Arrow_UpDown[];
extern uint8_t Arrow_Left[];
extern uint8_t Arrow_Right[];

//************************ Functions *******************************//
//BlueToothSetup.cpp
void BT_Setup();
uint8_t AT_CMD(String cmd);

//BuzzRing.cpp
void BuzzRing(uint8_t chs);
void LCD_DynamicPrint(uint8_t x, uint8_t y, char* str, uint32_t delayTime_ms);

//Calibration.cpp
void Calibrat_JSMP();
void Calibrat_JSMaxMin();
void Calibrat_ServoMP(int16_t XmpMin, int16_t XmpMax, int16_t YmpMin, int16_t YmpMax);
void Calibrat_MPU6050();

//DataProcess.cpp]
void TX_DataLoad();
void SendData();
void RX_DataLoad();
void RX_Transform();
void NRF_CheckConnection();

//DataStorage.cpp
void DataStorageHandle(uint8_t Select);
void ClearAllData();

//Display_MainScreen.cpp
void LCD_Display();
void ConnectInfo();
void CarTankInfo();
void ComputerInfo();
void SpiderInfo();
void CrawlerInfo();
void lcd_Space(uint8_t num);

//Display_OtherScreen.cpp
void OLED_Display();
void HMI_end();
void HMI_Display();
void HMI_ShowRPS();
int Get_RPS_PointerAngle(float rps);

//Joystick_Key_Read.cpp
void Read_Joystick();
int16_t JoystickMap(int16_t ADC_Value, int16_t MIN, int16_t MP, int16_t MAX);
void Read_Keys();
uint8_t BT_DOWN();

//LoadCtrlMode.cpp
void LoadCtrlMode(uint8_t cmd);

//Math.cpp
#define Limit(x,min,max) (x=constrain(x,min,max))
#define fLimit Limit
double PowMap(double value, double oriStart, double oriEnd, double st, double end, double stK, double endK);

//MEUN.cpp
void MenuRun();
void BasicPage();
void OptionsPage();
void ShowMenuCursor();
void NumClear(uint32_t Num, int16_t digit);
void MenuAutoClear();
void MenuCursorToPage();

//MPU6050.cpp
void Read_MPU6050();

//RC_Ctrl.cpp
void setup();
void loop();


#endif
