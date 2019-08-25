#ifndef __MUSICPLAY_H
#define __MUSICPLAY_H

#include "Tone.h"

typedef struct {
    uint32_t Freq;
    uint32_t Time;
} MusicCode_t;

class MusicPlay {
public:
    MusicPlay(uint8_t pin, const MusicCode_t* musiccode, uint32_t length);
    void Play();
	void Stop();
    bool Running(uint32_t MillisSeed);
private:
    uint8_t Pin;
    uint32_t Length;
    uint32_t NowPos;
    uint32_t NextTimePoint;
    const MusicCode_t* MusicCode;
};

#endif
