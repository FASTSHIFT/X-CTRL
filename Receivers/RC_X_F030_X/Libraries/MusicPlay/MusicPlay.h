#ifndef __MUSICPLAY_H
#define __MUSICPLAY_H

#if defined(ARDUINO)
#include "Arduino.h"
#else
#include "Tone.h"
#endif

#define MusicCodeSize(mc) (sizeof(mc)/sizeof(mc[0]))

typedef struct {
    uint16_t Freq;
    uint16_t Time;
	uint16_t Volume;
} MusicCode_t;

typedef void(*MusicPlayCallbackFunction_t)(uint32_t,uint16_t);

class MusicPlay {
public:
    MusicPlay(uint8_t pin);
	MusicPlay(MusicPlayCallbackFunction_t function);
	void SetMusicCode(const MusicCode_t* musiccode, uint16_t length);
	void SetMusicCode(MusicCode_t* musiccode, uint16_t length);
    void Play();
	void Stop();
    bool Running(uint32_t MillisSeed);
private:
    uint8_t Pin;
    uint16_t Length;
    uint16_t NowPos;
    uint32_t NextTimePoint;
    MusicCode_t* MusicCode;
	MusicPlayCallbackFunction_t MusicPlayFunction;
};

#endif
