#include "FileGroup.h"

typedef enum {
    L1 = 262,
    L1h = 277,
    L2 = 294,
    L2h = 311,
    L3 = 330,
    L4 = 349,
    L4h = 370,
    L5 = 392,
    L5h = 415,
    L6 = 440,
    L6h = 466,
    L7 = 494,
    M1 = 523,
    M1h = 554,
    M2 = 587,
    M2h = 622,
    M3 = 659,
    M4 = 698,
    M4h = 740,
    M5 = 784,
    M5h = 831,
    M6 = 880,
    M6h = 932,
    M7 = 988,
    H1 = 1046,
    H1h = 1109,
    H2 = 1175,
    H2h = 1245,
    H3 = 1318,
    H4 = 1397,
    H4h = 1480,
    H5 = 1568,
    H5h = 1661,
    H6 = 1760,
    H6h = 1865,
    H7 = 1976,
} Tone_TypeDef;

#define speed 0.5

const MusicCode_t Nokia[] = {
    H5, 200 * speed, 100,
    H4, 200 * speed, 100,
    M6, 350 * speed, 100,
    M7, 350 * speed, 100,
    H4, 200 * speed, 100,
    H3, 200 * speed, 100,
    M4, 350 * speed, 100,
    M5, 350 * speed, 100,
    H2, 200 * speed, 100,
    H1, 200 * speed, 100,
    M3, 350 * speed, 100,
    M5, 350 * speed, 100,
    H1, 500 * speed, 100,
    0, 0, 0
};

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
    static uint8_t Sound_Now, Sound_Last = !Sound_Now;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
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
        if(Sound_Now != Sound_Last)
        {
            mp.SetMusicCode(mc2, MusicCodeSize(mc2));
            mp.Play();
            Sound_Last = Sound_Now;
        }

    }
    mp.Running(millis());
}
