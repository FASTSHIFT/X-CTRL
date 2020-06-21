#include "FileGroup.h"
#include "ComPrivate.h"

MillisTaskManager mtm(TP_MAX, true);
USBHID HID;

bool UseMtm = true;

void setup()
{
  disableDebugPorts();

  StorageDataReg(Sim_ObjectSelect);
  EEPROM_Handle(EEPROM_Chs::ReadData);

  Sim_DeviceInit();

  pinMode(LED_Pin, OUTPUT);
  pinMode(KEY_Pin, INPUT_PULLUP);
  pinMode(NRF_IRQ_Pin, INPUT_PULLUP);
  pinMode(RandomSeed_Pin, INPUT_ANALOG);
  __LoopExecute((togglePin(LED_Pin), delay(80)), 5);
  randomSeed(analogRead(RandomSeed_Pin) * 1000);
  attachInterrupt(KEY_Pin, When_KeyPressEvent, FALLING);

  Com_Init();

  if (UseMtm)
  {
    mtm.TaskRegister(TP_NRF_Handler, Task_ComHandler, 10);
    mtm.TaskRegister(TP_Sim, Sim_DeviceProcess, 10);
    mtm.TaskRegister(TP_InfoLED, Task_InfoLED, 500);
  }

  while (!RCX::Handshake::Process())
  {
    __IntervalExecute(togglePin(LED_Pin), 2000);
  }
}

void loop()
{
  UseMtm ? mtm.Running(millis()) : Sim_DeviceProcess();
}

static void When_KeyPressEvent()
{
  Sim_SetNextObj();
  EEPROM_Handle(EEPROM_Chs::SaveData);
  NVIC_SystemReset();
}
