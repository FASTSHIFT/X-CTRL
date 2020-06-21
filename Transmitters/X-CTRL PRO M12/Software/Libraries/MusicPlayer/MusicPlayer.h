#ifndef __MUSICPLAYER_H
#define __MUSICPLAYER_H

#include <stdint.h>

namespace MusicPlayer_Type
{
typedef struct
{
    uint16_t Freq;
    uint16_t Time;
    uint16_t Volume;
} MusicCode_t;
}

class MusicPlayer
{
    typedef void(*MusicPlayCallbackFunction_t)(uint32_t, uint16_t);
public:
    MusicPlayer(MusicPlayCallbackFunction_t function);
    void SetMusicCode(MusicPlayer_Type::MusicCode_t* musiccode, uint16_t length);
    inline void SetMusicCode(const MusicPlayer_Type::MusicCode_t* musiccode, uint16_t length)
    {
        SetMusicCode((MusicPlayer_Type::MusicCode_t*)musiccode, length);
    }

    void Play();
    inline void Play(const MusicPlayer_Type::MusicCode_t* musiccode, uint16_t length)
    {
        SetMusicCode((MusicPlayer_Type::MusicCode_t*)musiccode, length);
        Play();
    }
    void Stop();
    void SetSpeed(float speed);
    bool Running(uint32_t MillisSeed);
private:
    uint16_t Length;
    uint16_t NowPos;
    uint32_t NextTimePoint;
    float Speed;
    MusicPlayer_Type::MusicCode_t* MusicCode;
    MusicPlayCallbackFunction_t MusicPlay_Callback;
};

#endif
