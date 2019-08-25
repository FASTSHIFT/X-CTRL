#include "FileGroup.h"
#include "MusicPlayer.h"

bool State_BuzzSound = ON;
bool State_MusicPlayer = OFF;

static MusicPlayer player(Buzz_Pin);

static const MusicPlayer_Type::MusicCode_t StartUpMc[] = {
    {523, 100, 100},
    {880, 120, 100},
    {659, 140, 100},
    {0,  1400, 100},
    {659, 140, 100},
    {880, 120, 100},
    {523, 100, 100},
    0
};

static const MusicPlayer_Type::MusicCode_t ErrorMc[] = {
    {100, 80, 100},
    {0,   80, 100},
    {100, 80, 100},
    0
};

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

void Task_MusicPlayerRunning()
{
    State_MusicPlayer = player.Running(millis());
}

static void SignalChangeEvent(uint8_t isConnect)
{
    if(isConnect == 2)
    {
        player.Play(ConnectMc, __Sizeof(ConnectMc));
    }
    else if(isConnect == 1)
    {
        player.Play(UnstableConnectMc, __Sizeof(UnstableConnectMc));
    }
    else
    {
        player.Play(DisconnectMc, __Sizeof(DisconnectMc));
    }
}

void Task_BuzzSignalMonitor()
{
    if(!State_BuzzSound)return;
    static uint8_t ConnectState = 0;
    static int16_t NRF_SignalStrength_Last = 0;
    static uint32_t UntableSignalStrength_TimePoint = 0;
    if(ABS(NRF_SignalStrength - NRF_SignalStrength_Last) > 50 && NRF_SignalStrength < 90)
    {
        ConnectState = 1;
        UntableSignalStrength_TimePoint = millis();
        NRF_SignalStrength_Last = NRF_SignalStrength;
    }

    if(NRF_SignalStrength < 10 && millis() - UntableSignalStrength_TimePoint > 3000)
        ConnectState = 0;
    else if(NRF_SignalStrength > 90 && millis() - UntableSignalStrength_TimePoint > 3000)
        ConnectState = 2;

    if(!State_MusicPlayer)
        __EventMonitor(ConnectState, SignalChangeEvent(ConnectState));
}

void BuzzMusic(uint8_t music)
{
    if(!State_BuzzSound)return;

    switch (music)
    {
    case 0:
        player.Play(StartUpMc, __Sizeof(StartUpMc));
        break;
    case 1:
        player.Play(ErrorMc, __Sizeof(ErrorMc));
        break;

    case 2:
        player.Play(ConnectMc, __Sizeof(ConnectMc));
        break;

    case 3:
        player.Play(UnstableConnectMc, __Sizeof(UnstableConnectMc));
        break;
    
    case 4:
        player.Play(DisconnectMc, __Sizeof(DisconnectMc));
        break;
    }
}

void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!State_BuzzSound)return;
    tone(Buzz_Pin, freq, time);
}
