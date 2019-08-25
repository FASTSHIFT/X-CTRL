#include "FileGroup.h"
#include "MusicPlayer.h"
#include "ComPrivate.h"

using namespace MusicPlayer_Type;

//#define Volume_MAX 101

//static void MusicPlay_Callback(uint32_t freq, uint16_t volume)
//{
//    PIN_MAP[Buzz_Pin].TIMx->PSC = (F_CPU / Volume_MAX / freq) - 1;
//    pwmWrite(Buzz_Pin, volume);
//}

#define PLAY_MUSIC(mc) player.Play(mc,__Sizeof(mc))
#define ADD_MUSIC(mc) {mc,__Sizeof(mc)}

/*音符频率对应表*/
typedef enum
{
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

/*实例化音乐播放器*/
static MusicPlayer player(Buzz_Pin);

/*蜂鸣器使能控制*/
bool State_BuzzSound = ON;

/*音乐播放器状态指示*/
static bool State_MusicPlayer = OFF;

/*开机音*/
static const MusicCode_t MC_StartUp[] =
{
    {M1, 80, 100},
    {M6, 80, 100},
    {M3, 80, 100},
    0
};

/*错误提示音*/
static const MusicCode_t MC_Error[] =
{
    {100, 80, 100},
    {0,   80, 100},
    {100, 80, 100},
    0
};

/*连接成功音*/
static const MusicCode_t MC_Connect[] =
{
    {H1, 80, 100},
    {H2, 80, 100},
    {H3, 80, 100},
    0
};

/*断开连接音*/
static const MusicCode_t MC_Disconnect[] =
{
    {H3, 80, 100},
    {H2, 80, 100},
    {H1, 80, 100},
    0
};

/*信号不稳定提示音*/
static const MusicCode_t MC_UnstableConnect[] =
{
    {H1, 80, 100},
    {0, 80, 100},
    {H1, 80, 100},
    0
};

/*充电启动提示音*/
static const MusicCode_t MC_BattChargeStart[] =
{
    {L1, 80, 100},
    {L3, 80, 100},
    0
};

/*充电关闭提示音*/
static const MusicCode_t MC_BattChargeEnd[] =
{
    {L3, 80, 100},
    {L1, 80, 100},
    0
};

/*设备插入提示音*/
static const MusicCode_t MC_DeviceInsert[] =
{
    /*C4,A3,F3,F4*/
    {M1, 180, 100},
    {L6, 80, 100},
    {L4, 80, 100},
    {M4, 160, 100},
    0
};

/*设备拔出提示音*/
static const MusicCode_t MC_DevicePullout[] =
{
    /*A4,F4,E4*/
    {L6, 80, 100},
    {L4, 80, 100},
    {L3, 80, 100},
    0
};

/*列表数据类型定义*/
typedef struct
{
    const MusicCode_t *mc;
    uint16_t length;
} MC_List_t;

/*提示音地址储存列表*/
static const MC_List_t MC_List[MC_Type::MC_MAX] = {
    ADD_MUSIC(MC_StartUp),
    ADD_MUSIC(MC_Error),
    ADD_MUSIC(MC_Connect),
    ADD_MUSIC(MC_Disconnect),
    ADD_MUSIC(MC_UnstableConnect),
    ADD_MUSIC(MC_BattChargeStart),
    ADD_MUSIC(MC_BattChargeEnd),
    ADD_MUSIC(MC_DeviceInsert),
    ADD_MUSIC(MC_DevicePullout)
};

/*信号状态指示改变事件*/
static void SignalChangeEvent(uint8_t isConnect)
{
    if(isConnect == 2)
    {
        BuzzMusic(MC_Type::MC_Connect);
        XFS_Speak("已连接");
        XFS_Speak("信号强度%d%%", NRF_SignalStrength);
    }
    else if(isConnect == 1)
    {
        BuzzMusic(MC_Type::MC_UnstableConnect);
        XFS_Speak("警告:信号不稳定");
        XFS_Speak("信号强度%d%%", NRF_SignalStrength);
    }
    else
    {
        BuzzMusic(MC_Type::MC_Disconnect);
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
  * @brief  信号状态监视任务
  * @param  无
  * @retval 无
  */
void Task_SignalMonitor()
{
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
    if(!State_BuzzSound) //是否使能蜂鸣器
        return;

    if(music >= MC_Type::MC_MAX) //数组访问是否越界
        return;

    /*播放器播放音乐*/
    player.Play(MC_List[music].mc, MC_List[music].length);
}

/**
  * @brief  往蜂鸣器输出指定频率和持续时间的方波信号(非阻塞式)
  * @param  freq:频率(Hz)
  * @param  time:持续时间(ms)
  * @retval 无
  */
void BuzzTone(uint32_t freq, uint32_t time)
{
    if(!State_BuzzSound)
        return;
    
    tone(Buzz_Pin, freq, time);
}
