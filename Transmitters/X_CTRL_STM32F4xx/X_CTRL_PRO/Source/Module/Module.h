#ifndef __MOUDLE_H
#define __MOUDLE_H

#include "stdint.h"

/*Motor*/
void Motor_Vibrate(float strength, uint32_t time);

/*Power*/
extern float BattCurret, BattVoltage, BattVoltageOc;
void Power_Shutdown();

/*Audio*/
void Audio_SetEnable(bool en);
void Wav_BufferUpdate();
void Init_WaveTest();

/*Text*/
char * TextGetBuff(uint32_t start = 0);
uint32_t TextGetSize();
void TextClear(uint8_t n = 0);

#endif
