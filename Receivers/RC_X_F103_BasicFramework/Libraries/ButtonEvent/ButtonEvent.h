#ifndef __BUTTONEVENT_H
#define __BUTTONEVENT_H

#include "Arduino.h"

typedef void(*ButtonEvent_FuncCallBack_t)(void);

class ButtonEvent{
public:
	ButtonEvent(uint8_t Pin,bool Button_NoPressState = 0,uint16_t LongPressTimeMs_Set = 500);
	ButtonEvent(bool NoPressState = 0, uint16_t LongPressTimeMs_Set = 500);
	void Init(uint8_t PinMode = INPUT);
	void EventAttach_Press(ButtonEvent_FuncCallBack_t Function);
	void EventAttach_LongPress(ButtonEvent_FuncCallBack_t Function);
	void EventAttach_Release(ButtonEvent_FuncCallBack_t Function);
	void EventAttach_Change(ButtonEvent_FuncCallBack_t Function);
	void Read_WipeShake();
	void Read_NoWipeShake(uint8_t NowState);
	void Remap_Pin(uint8_t Pin,uint8_t PinMode = INPUT);
	bool GetState();
	void ClearState();
	operator bool() { return Button_NowState;};
private:
	uint8_t Button_Pin;
	bool Button_NoPressState;
	bool Button_NowState;
	uint16_t LongPressTimeMs;

	uint32_t PressKey_TimePoint;
	bool IS_Pressed;

	ButtonEvent_FuncCallBack_t ButtonPress_Function;
	ButtonEvent_FuncCallBack_t ButtonLongPress_Function;
	ButtonEvent_FuncCallBack_t ButtonRelease_Function;
	ButtonEvent_FuncCallBack_t ButtonChange_Function;
};

#endif
