#include "MusicPlayer.h"

using namespace MusicPlayer_Type;

MusicPlayer::MusicPlayer(MusicPlayCallbackFunction_t function)
{
    MusicPlay_Callback = function;
    MusicCode = 0;
    Speed = 1.0;
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
    if(speed < 0.0f)
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
    if(!MusicPlay_Callback)
        return false;
    
    if(NowPos < Length)
    {
        if(MillisSeed > NextTimePoint)
        {
            MusicPlay_Callback(MusicCode[NowPos].Freq, MusicCode[NowPos].Volume);

            NextTimePoint = MillisSeed + MusicCode[NowPos].Time * (1.0f / Speed);
            NowPos++;
        }
        return true;
    }
    else if(NowPos == Length && MillisSeed > NextTimePoint)
    {
        MusicPlay_Callback(0, 0);
        NowPos++;
    }
    return false;
}
