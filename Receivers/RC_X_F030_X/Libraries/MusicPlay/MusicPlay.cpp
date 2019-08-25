#include "MusicPlay.h"

MusicPlay::MusicPlay(uint8_t pin)
{
    Pin = pin;
	MusicPlayFunction = 0;
	MusicCode = 0;
}

MusicPlay::MusicPlay(MusicPlayCallbackFunction_t function)
{
	MusicPlayFunction = function;
	MusicCode = 0;
}

void MusicPlay::SetMusicCode(const MusicCode_t* musiccode, uint16_t length)
{
	if((MusicCode_t*)musiccode != MusicCode)
	{
		MusicCode = (MusicCode_t*)musiccode;
		Length = length;
		NowPos = length;
	}
}

void MusicPlay::SetMusicCode(MusicCode_t* musiccode, uint16_t length)
{
	if(musiccode != MusicCode)
	{
		MusicCode = musiccode;
		Length = length;
		NowPos = length;
	}
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
			if(MusicPlayFunction)
			{
				MusicPlayFunction(MusicCode[NowPos].Freq, MusicCode[NowPos].Volume);
			}
			else
			{
				tone(Pin, MusicCode[NowPos].Freq);
			}
            NextTimePoint = MillisSeed + MusicCode[NowPos].Time;
            NowPos++;		
        }
		return true;
    }
	else if(NowPos == Length)
    {
		if(MusicPlayFunction)
		{
			MusicPlayFunction(0,0);
		}
		else
		{
			noTone(Pin);
		}
        NowPos++;
    }
    return false;
}
