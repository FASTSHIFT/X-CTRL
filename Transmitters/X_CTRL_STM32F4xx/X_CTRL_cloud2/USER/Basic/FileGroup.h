#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//*************** STM32 Core *****************//
#include "delay.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "exti.h"
#include "timer.h"

//************* Arduino API **************//
#include "Arduino.h"
#include "WMath.h"
#include "Tone.h"

//************* User Libraries ************//
#include "ButtonEvent.h"
#include "EncoderEvent.h"
#include "MillisTaskManager.h"
#include "RC_Protocol.h"
#include "CommonMacro.h"
#include "Config.h"
#include "shellclient.h"
#include "nrf.h"

namespace EEPROM_Chs {
typedef enum {
    ReadData,
    SaveData,
    ClearData
} EEPROM_Chs_t;
}

//************* Object & Struct & Value**************///*Object*/
extern ButtonEvent btEcd;
extern EncoderEvent ecd;
extern ButtonEvent btMK[9];

/*Struct*/
extern Joystick_t JS_L;
extern Joystick_t JS_R;
extern ShellClient client;

/*Value*/
extern bool State_BuzzSound;
extern float BattUsage;
extern float BattVoltage;

//****************** Functions********************//
void BuzzMusic(uint8_t music);
void BuzzTone(uint32_t freq, uint32_t time);
void BuzzTone(uint32_t freq);
void ButtonEventMonitor();

#define Client_Timeout				100
#define Client_CallTimeout		3000
void Client_Handle();
uint32_t Client_Call(const char *fname, int pSize, void *params, void *ret);
typedef struct
{
	float Yaw;								/* 航向值 单位 degree */
	float Pitch;							/* 俯仰值 单位 degree */
	float Roll;								/* 横滚值 单位 degree */
} AttitudePoint_TypeDef;
/* 指示一个从某位置到指定位置的轨迹/节点 
 * ----------------------------------------------
 * 
 */
typedef struct
{
	char Name[12];												/* 指示节点的名称 */
	AttitudePoint_TypeDef Attitude;				/* 指示节点稳定时的姿态 */
	float SettleTime;											/* 指示节点停留时间 */
	uint8_t TrackAnimationIndex;					/* 指示使用资源列表中的哪一个动画实例来切换到此节点 */
} TrackPoint_TypeDef;

/* 指示一个动画过程
 * ----------------------------------------------
 * 
 */
typedef struct
{
	char Name[11];												/* 指示动画的名称 */
	uint8_t TrackTypeID;									/* 指示该动画实例使用的基本动画生成器 ID */
	float Duration;												/* 指示该动画实例的持续时间 */
	float Params[4];											/* 指示该动画实例可能需要使用的参数 */
} TrackAnimation_TypeDef;


void EncoderMonitor();
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);
bool EEPROM_Register(void *pdata, uint16_t size);

void Init_X_CTRL();
bool Init_Value();
void Init_Buzz();
void Init_BottonEvent();
void Init_Display();
void Init_EncoderEvent();
void Init_GUI();
bool Init_SD();
void Init_Client();

void Thread_GUI();
void Task_SensorUpdate();
void Task_MusicPlayerRunning();

#endif
