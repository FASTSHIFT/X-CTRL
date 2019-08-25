#include "FileGroup.h"
#include "MusicPlayer.h"
#include "ComPrivate.h"

//#define Volume_MAX 101

//static void MusicPlay_Callback(uint32_t freq, uint16_t volume)
//{
//    PIN_MAP[Buzz_Pin].TIMx->PSC = (F_CPU / Volume_MAX / freq) - 1;
//    pwmWrite(Buzz_Pin, volume);
//}

/*实例化音乐播放器*/
static MusicPlayer player(Buzz_Pin);

/*蜂鸣器使能控制*/
bool State_BuzzSound = ON;

/*音乐播放器状态指示*/
static bool State_MusicPlayer = OFF;

/*开机音*/
static const MusicPlayer_Type::MusicCode_t StartUpMc[] = {
    {523, 80, 100},
    {880, 80, 100},
    {659, 80, 100},
    0
};

/*错误提示音*/
static const MusicPlayer_Type::MusicCode_t ErrorMc[] = {
    {100, 80, 100},
    {0,   80, 100},
    {100, 80, 100},
    0
};

/*连接成功音*/
static const MusicPlayer_Type::MusicCode_t ConnectMc[] = {
    {1046, 80, 100},
    {1175, 80, 100},
    {1318, 80, 100},
    0
};

/*断开连接音*/
static const MusicPlayer_Type::MusicCode_t DisconnectMc[] = {
    {1318, 80, 100},
    {1175, 80, 100},
    {1046, 80, 100},
    0
};

/*信号不稳定提示音*/
static const MusicPlayer_Type::MusicCode_t UnstableConnectMc[] = {
    {1046, 80, 100},
    {0, 80, 100},
    {1046, 80, 100},
    0
};

/**
  * @brief  初始化蜂鸣器
  * @param  无
  * @retval 无
  */
void Init_Buzz()
{
    toneSetTimer(TIM_BuzzTone);
    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);
//    PWM_Init(Buzz_Pin, Volume_MAX, 1000);
//    pwmWrite(Buzz_Pin, 0);
}

/**
  * @brief  音乐播放任务
  * @param  无
  * @retval 无
  */
void Task_MusicPlayerRunning()
{
    State_MusicPlayer = player.Running(millis());
}

/**
  * @brief  音乐播放
  * @param  music:音乐编号
  * @retval 无
  */
void BuzzMusic(uint8_t music)
{	
#define PLAY(m) player.Play(m, __Sizeof(m))
	
    if(!State_BuzzSound)//是否使能蜂鸣器
        return;

    switch (music)
    {
    case 0:
        PLAY(StartUpMc);
        break;
    case 1:
        PLAY(ErrorMc);
        break;

    case 2:
        PLAY(ConnectMc);
        break;

    case 3:
		PLAY(UnstableConnectMc);
        break;
    
    case 4:
        PLAY(DisconnectMc);
        break;
    }
}

/**
  * @brief  往蜂鸣器输出指定频率和持续时间的方波信号(非阻塞式)
  * @param  freq:频率(Hz)
  * @param  time:持续时间(ms)  
  * @retval 无
  */
void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!State_BuzzSound)return;
    tone(Buzz_Pin, freq, time);
}
