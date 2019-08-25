#include "FileGroup.h"
#include "MusicPlay.h"
#include "ComPrivate.h"

//const MusicCode_t mcNokia[] = {
//    H5, 200, 100,
//    H4, 200, 100,
//    M6, 350, 100,
//    M7, 350, 100,
//    H4, 200, 100,
//    H3, 200, 100,
//    M4, 350, 100,
//    M5, 350, 100,
//    H2, 200, 100,
//    H1, 200, 100,
//    M3, 350, 100,
//    M5, 350, 100,
//    H1, 500, 100,
//    0, 0, 0
//};

const MusicCode_t mc1[] = {
    1046, 80, 100,
    1175, 80, 100,
    1318, 80, 100,
    0, 0, 100,
};

const MusicCode_t mc2[] = {
    100, 80, 100,
    0, 80, 100,
    100, 80, 100,
    0, 0, 100,
};

MusicPlay mp(Buzz_Pin);

void Task_BuzzRing()
{
    enum {Start = 1, Connect, Disonnect, Error};
    static uint8_t Sound_Now = Start, Sound_Last = Sound_Now;
    static uint32_t LostSignal_TimePoint;
    if(ConnectState.IsConnect && ConnectState.IsCorrect && millis() - LostSignal_TimePoint > 20000)
    {
        Sound_Now = Connect;
        if(Sound_Now != Sound_Last)
        {
            mp.SetMusicCode(mc1, MusicCodeSize(mc1));
            mp.Play();
            Sound_Last = Sound_Now;
        }
    }
    else if(!ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Disonnect;
        LostSignal_TimePoint = millis();
        if(Sound_Now != Sound_Last)
        {
            mp.SetMusicCode(mc2, MusicCodeSize(mc2));
            mp.Play();
            Sound_Last = Sound_Now;
        }
    }
    mp.Running(millis());
}
