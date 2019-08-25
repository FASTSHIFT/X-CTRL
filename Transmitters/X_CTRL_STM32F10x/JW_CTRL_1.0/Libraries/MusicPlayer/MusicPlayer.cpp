#include "MusicPlayer.h"

using namespace MusicPlayer_Type;

MusicPlayer::MusicPlayer(uint8_t pin)
{
    Pin = pin;
	MusicPlayFunction = 0;
	MusicCode = 0;
    Speed = 1.0;
}

MusicPlayer::MusicPlayer(MusicPlayCallbackFunction_t function)
{
	MusicPlayFunction = function;
	MusicCode = 0;
    Speed = 1.0;
}

void MusicPlayer::SetMusicCode(const MusicCode_t* musiccode, uint16_t length)
{
	if((MusicCode_t*)musiccode != MusicCode)
	{
		MusicCode = (MusicCode_t*)musiccode;
		Length = length;
		NowPos = length;
	}
}

void MusicPlayer::SetMusicCode(MusicCode_t* musiccode, uint16_t length)
{
	if(musiccode != MusicCode)
	{
		MusicCode = musiccode;
		Length = length;
		NowPos = length;
	}
}

void MusicPlayer::SetSpeed(float speed)
{
    if(speed < 0.0)
        return;
    
    Speed = speed;
}

void MusicPlayer::Play()
{
    NowPos = 0;
}

void MusicPlayer::Stop()
{
	NowPos = Length;
}

bool MusicPlayer::Running(uint32_t MillisSeed)
{
    if(NowPos < Length)
    {
        if(MillisSeed > NextTimePoint)
        {
			if(MusicPlayFunction)
				MusicPlayFunction(MusicCode[NowPos].Freq, MusicCode[NowPos].Volume);
			else
				tone(Pin, MusicCode[NowPos].Freq);
                
            NextTimePoint = MillisSeed + MusicCode[NowPos].Time * (1.0 / Speed);
            NowPos++;		
        }
		return true;
    }
	else if(NowPos == Length)
    {
		if(MusicPlayFunction)
			MusicPlayFunction(0,0);
		else
			noTone(Pin);
        
        NowPos++;
    }
    return false;
}
