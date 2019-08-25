#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "arduino.h"

#define Buttons_MultiClickCycle		60
#define Buttons_LongPressCycle		200
#define Buttons_Count							10

#define Buttons_ClearAfterRead

void Buttons_Read();
void Buttons_Clear();
void Buttons_Init();
void Buttons_Update();

/* ------------------------- Events ------------------------- */
uint8_t Buttons_IsClicked(uint8_t index);
uint8_t Buttons_IsPressed(uint8_t index);
uint8_t Buttons_IsReleased(uint8_t index);
uint8_t Buttons_IsLongPressed(uint8_t index);
/* ------------------------- Status ------------------------- */
uint8_t Buttons_IsPressing(uint8_t index);
uint8_t Buttons_IsLongPressing(uint8_t index);

#endif

