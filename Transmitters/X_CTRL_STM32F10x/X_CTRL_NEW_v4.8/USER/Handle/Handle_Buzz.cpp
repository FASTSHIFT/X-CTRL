#include "FileGroup.h"
#include "MusicPlayer.h"
#include "ComPrivate.h"

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

static const MusicPlayer_Type::MusicCode_t UnstableConnectMc[] = {
    {1046, 80, 100},
    {0, 80, 100},
    {1046, 80, 100},
    0
};

/*信号状态指示改变事件*/
static void SignalChangeEvent(uint8_t isConnect)
{
    if(isConnect == 2)
    {
        player.Play(ConnectMc, __Sizeof(ConnectMc));//音乐播放
        XFS_Speak("已连接");
        XFS_Speak("信号强度%d%%", NRF_SignalStrength);
    }
    else if(isConnect == 1)
    {
        player.Play(UnstableConnectMc, __Sizeof(UnstableConnectMc));
        XFS_Speak("警告:信号不稳定");
        XFS_Speak("信号强度%d%%", NRF_SignalStrength);
    }
    else
    {
        player.Play(DisconnectMc, __Sizeof(DisconnectMc));
        XFS_Speak("连接断开");
    }
}

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
  * @brief  信号状态监视任务
  * @param  无
  * @retval 无
  */
void Task_SignalMonitor()
{
    if(!State_BuzzSound)//是否使能蜂鸣器
        return;
    
    /*连接状态指示*/
    static uint8_t ConnectState = 0;
    
    /*上一次的NRF信号强度*/
    static int16_t NRF_SignalStrength_Last = 0;
    
    /*当信号不稳定时的时间戳*/
    static uint32_t UntableSignalStrength_TimePoint = 0;
    
    /*判断信号是否稳定*/
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

    if(!State_MusicPlayer)
    {
        /*监控ConnectState变量，当其改变时触发SignalChangeEvent事件*/
        __ValueMonitor(ConnectState, SignalChangeEvent(ConnectState));
    }
}

/**
  * @brief  音乐播放
  * @param  music:音乐编号
  * @retval 无
  */
void BuzzMusic(uint8_t music)
{
    if(!State_BuzzSound)//是否使能蜂鸣器
        return;

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
