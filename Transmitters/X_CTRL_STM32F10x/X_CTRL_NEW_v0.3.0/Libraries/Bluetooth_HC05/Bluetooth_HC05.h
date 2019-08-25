#ifndef __Bluetooth_HC05
#define __Bluetooth_HC05

#include "Arduino.h"
#include "HardwareSerial.h"
#include "WString.h"

class Bluetooth_HC05{
public:
    Bluetooth_HC05(HardwareSerial &serialx, int16_t State_Pin = -1, int16_t EN_Pin = -1, int16_t PowerCtrl_Pin = -1);
    void Init(uint32_t baudRate);
	bool Power(bool state);
	
	bool AT_Enter();
	void AT_Exit();
	String AT_Cmd(String Cmd,uint32_t TimeOut = 3000);

	bool Set_Role(bool role,String name);
	bool Set_Password(String password);
	bool Set_BaudRate(uint32_t baudRate);

	bool Get_State();
    String Get_Password();
	uint32_t Get_BaudRate();
	
private:
	HardwareSerial *Serialx;
    int16_t State_Pin, EN_Pin, PowerCtrl_Pin;
	uint32_t BaudRate;
};

#endif
