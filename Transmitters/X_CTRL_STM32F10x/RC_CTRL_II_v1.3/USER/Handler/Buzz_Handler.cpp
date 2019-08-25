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

bool IsPWM_Mode = false;

#define Volume_MAX 101
TIM_TypeDef* Buzz_TIMx;
uint16_t Buzz_TIMx_Channel;

void MusicPlayUpdate_Callback(uint32_t freq, uint16_t volume)
{
    Buzz_TIMx->PSC = (F_CPU / Volume_MAX / freq) - 1;
    Buzz_TIMx->CCR3 = volume;
}
MusicPlay mp1(MusicPlayUpdate_Callback);

void MusicPlayAlwaysRunning()
{
    mp.Running(millis());
}

void Init_Buzz()
{
    if(IS_PWM_PIN(Buzz_Pin) && IsPWM_Mode)
    {
        PWM_Init(Buzz_Pin, Volume_MAX, 1000);
        pwmWrite(Buzz_Pin, 0);
        Buzz_TIMx = PIN_MAP[Buzz_Pin].TIMx;

        Timer_Init(TIM4, 10 * 1000, MusicPlayAlwaysRunning, 2, 2);
        TIM_Cmd(TIM4, ENABLE);
        IsPWM_Mode = true;
    }
    else
    {
        pinMode(Buzz_Pin, OUTPUT);
        IsPWM_Mode = false;
    }
}


const MusicCode_t Mc_Error[] = {
    {100, 100, 100},
    {0, 100, 0},
    {100, 100, 100},
    {0, 100, 0},
    0
};

const MusicCode_t Mc_Connect[] = {
    {1046, 80, 100},
    {1175, 80, 100},
    {1318, 80, 100},
    0
};

const MusicCode_t Mc_StartUp[] = {
    {523, 80, 100},
    {880, 80, 90},
    {659, 80, 80},
    {523, 80, 70},
    {880, 80, 60},
    {659, 80, 50},
    {523, 80, 40},
    {880, 80, 30},
    {659, 80, 20},
    {523, 80, 10},
    {880, 80, 10},
    {659, 80, 10},
    {523, 80, 5},
    {880, 80, 5},
    {659, 80, 5},
    {523, 80, 3},
    {880, 80, 3},
    {659, 80, 3},
    {523, 80, 2},
    {880, 80, 2},
    {659, 80, 2},
    {523, 80, 1},
    {880, 80, 1},
    {659, 80, 1},
    0
};

void BuzzSignalMonitor()
{
    if(!BuzzSound_State)return;
	
	static bool IsConnect = false,IsConnect_Last = IsConnect;
	if(SignalStrength < 20)
	{
		IsConnect = false;
		mp.SetMusicCode(DisconnectMc,MusicCodeSize(DisconnectMc));
	}
	else if(SignalStrength > 80)
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
        if(IsPWM_Mode)
        {
            mp.SetMusicCode(Mc_Error, MusicCodeSize(Mc_Error));
            mp.Play();
            break;
        }
        toneBlock(Buzz_Pin, 100, 100);
        delay(100);
        toneBlock(Buzz_Pin, 100, 100);
        break;

    case 2:
        if(IsPWM_Mode)
        {
            mp.SetMusicCode(Mc_Connect, MusicCodeSize(Mc_Connect));
            mp.Play();
            break;
        }
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
        if(IsPWM_Mode)
        {
            mp.SetMusicCode(Mc_StartUp, MusicCodeSize(Mc_StartUp));
            mp.Play();
            break;
        }
        toneBlock(Buzz_Pin, 523, 80);//1
        toneBlock(Buzz_Pin, 880, 80);//6
        toneBlock(Buzz_Pin, 659, 80);//3
        break;
    }
}

void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!BuzzSound_State || IsPWM_Mode)return;
    tone(Buzz_Pin, freq, time);
}
