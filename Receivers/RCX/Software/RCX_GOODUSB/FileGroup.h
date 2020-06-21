#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

//************* Arduino Library **************//
#include "Arduino.h"

//********** User Libraries *********//
#include "MillisTaskManager.h"
#include "CommonMacro.h"
#include <USBComposite.h>

//************* Objects *************//
extern MillisTaskManager mtm;
extern USBHID HID;

//*********** GPIO Define ***********//
#define LED_Pin		     PB7
#define KEY_Pin        PB6
#define RandomSeed_Pin PB0

#define NRF_IRQ_Pin		PA0
#define	NRF_MISO_Pin	PA1
#define NRF_MOSI_Pin	PA2
#define NRF_SCK_Pin		PA3
#define NRF_CSN_Pin		PA4
#define NRF_CE_Pin		PA5
/**NRF24L01 引脚分布图(模块正面,俯视)
   +---------------+-------+
   |               |       |
   |        _______|_______|
   |        |8:IRQ | 7:MISO|
   |        |6:MOSI| 5:SCK |
   |        |4:CSN | 3:CE  |
   |        |2:VCC | 1:GND |
   +---------------+-------+
*/

//*********** Value ***********//
#define digitalWrite_HIGH(pin) gpio_write_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit, 1)
#define digitalWrite_LOW(pin)  gpio_write_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit, 0)
#define digitalRead_FAST(pin)  gpio_read_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit)
#define togglePin(pin)         gpio_toggle_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit)

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))

#define NVIC_SystemReset() nvic_sys_reset()

namespace EEPROM_Chs {
typedef enum {
  ReadData,
  SaveData,
  ClearData
} EEPROM_Chs_t;
}

enum TaskPriority {
  TP_NRF_Handler,
  TP_Sim,
  TP_InfoLED,
  TP_MAX
};

typedef void (*voidFuncPtr_t)(void);
extern uint8_t Sim_ObjectSelect;
extern bool UseMtm;

//*********** Functions ***********//
bool Sim_RegFunc(uint8_t id, voidFuncPtr_t setupFunc, voidFuncPtr_t loopFunc);
void Sim_DeviceInit();
void Sim_DeviceProcess();
void Sim_SetNextObj();

void Task_ComHandler();
void Task_InfoLED();

bool EEPROM_Register(void *pdata, uint16_t size);
bool EEPROM_Handle(EEPROM_Chs::EEPROM_Chs_t chs);

#endif
