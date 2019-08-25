#include "MusicPlay.h"

MusicPlay::MusicPlay(uint8_t pin, const MusicCode_t* musiccode, uint32_t length)
{
    Pin = pin;
    MusicCode = musiccode;
    Length = length;
    NowPos = 0;
}

void MusicPlay::Play()
{
    NowPos = 0;
}

void MusicPlay::Stop()
{
	NowPos = Length;
}

bool MusicPlay::Running(uint32_t MillisSeed)
{
    if(NowPos < Length)
    {
        if(MillisSeed > NextTimePoint)
        {
            tone(Pin, MusicCode[NowPos].Freq);
            NextTimePoint = MillisSeed + MusicCode[NowPos].Time;
            NowPos++;
			
        }
		return true;
    }
	else return false;
}
