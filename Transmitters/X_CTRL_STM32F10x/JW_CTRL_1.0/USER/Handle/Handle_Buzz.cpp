#include "FileGroup.h"
#include "MusicPlayer.h"
#include "ComPrivate.h"

bool State_BuzzSound = OFF;

static MusicPlayer mp(Buzz_Pin);

static const MusicPlayer_Type::MusicCode_t ConnectMc[] = {
    {1046, 80, 100},
    {1175, 80, 100},
    {1318, 80, 100},
    0
};

static const MusicPlayer_Type::MusicCode_t DisconnectMc[] = {
    {1318, 80, 100},
    {1175, 80, 100},
    {1046, 80, 100},
    0
};

static const MusicPlayer_Type::MusicCode_t UnstableConnectMc[] = {
    {1046, 80, 100},
    {0, 80, 100},
    {1046, 80, 100},
    0
};

static void SignalChangeEvent(uint8_t isConnect)
{
    if(isConnect == 2)
    {
        mp.SetMusicCode(ConnectMc, __Sizeof(ConnectMc));
    }
    else if(isConnect == 1)
    {
        mp.SetMusicCode(UnstableConnectMc, __Sizeof(UnstableConnectMc));
    }
    else
    {
        mp.SetMusicCode(DisconnectMc, __Sizeof(DisconnectMc));
    }

    mp.Play();
}

void Task_SignalMonitor()
{
    if(!State_BuzzSound)return;
    static uint8_t ConnectState = 0;
    static int16_t NRF_SignalStrength_Last = 0;
    static uint32_t UntableSignalStrength_TimePoint = 0;
    if(ABS(NRF_SignalStrength - NRF_SignalStrength_Last) > 10 && NRF_SignalStrength < 90)
    {
        ConnectState = 1;
        UntableSignalStrength_TimePoint = millis();
        NRF_SignalStrength_Last = NRF_SignalStrength;
    }

    if(NRF_SignalStrength < 10 && millis() - UntableSignalStrength_TimePoint > 3000)
        ConnectState = 0;
    else if(NRF_SignalStrength > 90 && millis() - UntableSignalStrength_TimePoint > 5000)
        ConnectState = 2;

    if(!mp.Running(millis()))
        __EventMonitor(ConnectState, SignalChangeEvent(ConnectState));
}

void BuzzMusic(uint8_t music)
{
    if(!State_BuzzSound)return;

    switch (music)
    {
    case 1:
        toneBlock(Buzz_Pin, 100, 100);
        delay(100);
        toneBlock(Buzz_Pin, 100, 100);
        break;

    case 2:
        toneBlock(Buzz_Pin, 1046, 80);//80
        toneBlock(Buzz_Pin, 1175, 80);
        toneBlock(Buzz_Pin, 1318, 80);
        break;

    case 3:
        toneBlock(Buzz_Pin, 523, 60);
        toneBlock(Buzz_Pin, 587, 60);
        toneBlock(Buzz_Pin, 659, 60);
        toneBlock(Buzz_Pin, 698, 60);
        toneBlock(Buzz_Pin, 523, 60);
        toneBlock(Buzz_Pin, 587, 60);
        toneBlock(Buzz_Pin, 659, 60);
        toneBlock(Buzz_Pin, 698, 60);
        break;

    case 4:
        toneBlock(Buzz_Pin, 523, 80);//1
        toneBlock(Buzz_Pin, 880, 80);//6
        toneBlock(Buzz_Pin, 659, 80);//3
        break;
    }
}

void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!State_BuzzSound)return;
    tone(Buzz_Pin, freq, time);
}
