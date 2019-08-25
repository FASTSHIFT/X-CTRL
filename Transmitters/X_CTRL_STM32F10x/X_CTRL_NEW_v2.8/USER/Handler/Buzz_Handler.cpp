#include "FileGroup.h"
#include "MusicPlay.h"

bool BuzzSound_State = ON;
MusicPlay mp(Buzz_Pin);
MusicCode_t ConnectMc[] = {
    {1046, 80, 100},
    {1175, 80, 100},
    {1318, 80, 100},
	0
};
MusicCode_t DisconnectMc[] = {
    {1318, 80, 100},
    {1175, 80, 100},
    {1046, 80, 100},
	0
};

void BuzzSignalMonitor()
{
    if(!BuzzSound_State || BattVoltage < 3.1)return;
	
	static bool IsConnect = false,IsConnect_Last = IsConnect;
	if(NRF_SignalStrength < 20)
	{
		IsConnect = false;
		mp.SetMusicCode(DisconnectMc,MusicCodeSize(DisconnectMc));
	}
	else if(NRF_SignalStrength > 80)
	{
		IsConnect = true;
		mp.SetMusicCode(ConnectMc,MusicCodeSize(ConnectMc));
	}
	
    if(!mp.Running(millis()))
	{
		if(IsConnect != IsConnect_Last)
		{
			mp.Play();
			IsConnect_Last = IsConnect;
		}
	}
}

void BuzzMusic(uint8_t music)
{
    if(!BuzzSound_State)return;

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
    if(!BuzzSound_State)return;
    tone(Buzz_Pin, freq, time);
}
