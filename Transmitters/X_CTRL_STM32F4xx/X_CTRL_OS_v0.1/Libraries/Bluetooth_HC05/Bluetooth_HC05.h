#ifndef __Bluetooth_HC05
#define __Bluetooth_HC05

#include "Arduino.h"
#include "HardwareSerial.h"
#include "WString.h"

class Bluetooth_HC05{
public:
    Bluetooth_HC05(HardwareSerial *serialx, int16_t State_Pin = -1, int16_t EN_Pin = -1, int16_t PowerCtrl_Pin = -1);
    void Init(uint32_t baudRate);
	bool Power(bool state);
	
	bool AT_Enter();
	void AT_Exit();
	String AT_Cmd(String Cmd,uint32_t TimeOut = 3000);

	bool SetRole(bool role);
	bool SetName(String name);
	bool SetPassword(String password);
	bool SetBaudRate(uint32_t baudRate);

	bool GetState();
	int8_t GetRole();
	bool GetName(String *name);
    bool GetPassword(String *password);
	bool GetBaudRate(uint32_t *baudRate);
	
private:
	HardwareSerial *Serialx;
    int16_t State_Pin, EN_Pin, PowerCtrl_Pin;
	uint32_t BaudRate;
};

#endif
