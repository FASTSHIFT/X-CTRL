#ifndef __BSP_H
#define __BSP_H

#include "Basic/SysConfig.h"
#include "Basic/CommonMacro.h"
#include "Arduino.h"

#define DEBUG_FUNC_LOG() Serial.printf("%s (%0.3fms)\r\n", __FUNCTION__, micros()/1000.0f)

/*Brightness*/
uint16_t Brightness_GetValue();
void Brightness_SetValue(int16_t val);
void Brightness_SetGradual(uint16_t target, uint16_t time = 500);

/*Buttons*/
#include "ButtonEvent/ButtonEvent.h"
#include "SwitchEvent/SwitchEvent.h"

enum{
    BTN_IDX_UP,
    BTN_IDX_DOWN,
    BTN_IDX_OK,
    BTN_IDX_BACK,
    BTN_IDX_UPL,
    BTN_IDX_DOWNL,
    BTN_IDX_POWER,
    BTN_IDX_MAX
};
extern ButtonEvent btGrp[BTN_IDX_MAX];
#define btUP    btGrp[BTN_IDX_UP]
#define btDOWN  btGrp[BTN_IDX_DOWN]
#define btOK    btGrp[BTN_IDX_OK]
#define btBACK  btGrp[BTN_IDX_BACK]
#define btUPL   btGrp[BTN_IDX_UPL]
#define btDOWNL btGrp[BTN_IDX_DOWNL]
#define btPOWER btGrp[BTN_IDX_POWER]

enum{
    SW_IDX_E,
    SW_IDX_F,
    SW_IDX_G,
    SW_IDX_H,
    SW_IDX_MAX
};
extern SwitchEvent swGrp[SW_IDX_MAX];
#define swE swGrp[SW_IDX_E]
#define swF swGrp[SW_IDX_F]
#define swG swGrp[SW_IDX_G]
#define swH swGrp[SW_IDX_H]

void Button_Init();
void Button_Update();

/*Buzzer*/
namespace MC_Type {
typedef enum {
    MC_StartUp,
    MC_Error,
    MC_Connect,
    MC_Disconnect,
    MC_UnstableConnect,
    MC_BattChargeStart,
    MC_BattChanargeEnd,
    MC_DeviceInsert,
    MC_DevicePullout,
    MC_NoOperationWarning,
    MC_MAX
}MusicCode_ID_Type;
}
void Buzz_Init();
void Buzz_Update();
void Buzz_PlayMusic(uint8_t music);
void Buzz_Tone(uint32_t freq, uint32_t time);

/*Joystick*/
void Joystick_Init();
void Joystick_Update();

/*IMU*/
void IMU_Init();
void IMU_Update();

/*EEPROM*/
#define EEPROM_REG_VALUE(data) EEPROM_Register(&(data), sizeof(data))
uint8_t EEPROM_Init();
bool EEPROM_Register(void *pdata, uint16_t size);
bool EEPROM_ReadAll();
bool EEPROM_SaveAll();

/*I2C*/
void I2C_Scan();
void I2C_SetLock(bool lock);
bool I2C_GetLocked();

/*Power*/
void Power_Init();
void Power_GetInfo(float* battCurrent,float* battVoltage,float* battVoltageOc);
float Power_GetBattUsage();
void Power_Update();
void Power_Shutdown();
void Power_SetLedState(bool state);

/*Bluetooth*/
void Bluetooth_Init();

/*Motor*/
void Motor_Init();
void Motor_Update();
void Motor_SetEnable(bool en);
void Motor_Vibrate(float strength, uint32_t time);
void Motor_WriteData(int16_t data);
void Motor_Tone(float freq);
void Motor_Tone(float freq, uint32_t time);
void Motor_ToneSetVolume(uint16_t volume);
uint16_t Motor_ToneGetVolume();
void Motor_NoTone();

/*BigMotor*/
void BigMotor_Init();
void BigMotor_SetEnable(bool en);
bool BigMotor_GetEnable();
void BigMotor_SetValue(uint16_t value);

#endif
