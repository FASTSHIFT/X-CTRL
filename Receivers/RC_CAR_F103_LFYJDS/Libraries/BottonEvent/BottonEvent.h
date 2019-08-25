#ifndef __BOTTONEVENT_H
#define __BOTTONEVENT_H

#include "Arduino.h"

typedef void(*BottonEvent_FuncCallBack_t)(void);

class BottonEvent{
public:
	BottonEvent(uint8_t Pin,bool Botton_NoPressState = 0,uint16_t LongPressTimeMs_Set = 500);
	BottonEvent(bool NoPressState = 0, uint16_t LongPressTimeMs_Set = 500);
	void Init(uint8_t PinMode = INPUT);
	void EventAttach_Press(BottonEvent_FuncCallBack_t Function);
	void EventAttach_LongPress(BottonEvent_FuncCallBack_t Function);
	void EventAttach_Release(BottonEvent_FuncCallBack_t Function);
	void EventAttach_Change(BottonEvent_FuncCallBack_t Function);
	void Read_WipeShake();
	void Read_NoWipeShake(uint8_t NowState);
	void Remap_Pin(uint8_t Pin,uint8_t PinMode = INPUT);
	bool GetState();
	void ClearState();
	operator bool() { return Botton_NowState;};
private:
	uint8_t Botton_Pin;
	bool Botton_NoPressState;
	bool Botton_NowState;
	uint16_t LongPressTimeMs;

	uint32_t PressKey_TimePoint;
	bool IS_Pressed;

	BottonEvent_FuncCallBack_t BottonPress_Function;
	BottonEvent_FuncCallBack_t BottonLongPress_Function;
	BottonEvent_FuncCallBack_t BottonRelease_Function;
	BottonEvent_FuncCallBack_t BottonChange_Function;
};

#endif
