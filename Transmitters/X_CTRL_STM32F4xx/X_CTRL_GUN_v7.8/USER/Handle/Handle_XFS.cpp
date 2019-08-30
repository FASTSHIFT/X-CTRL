#include "FileGroup.h"
#include "XFS.h"
#include <stdio.h>
#include <stdarg.h>
#include "FifoQueue.h"

/*实例化语音合成对象*/
XFS5152CE xfs(&XFS_SERIAL);

/*实例化语音合成队列，队列长度5*/
FifoQueue<String> SpeakStr(5);

/*语音合成允许标志位*/
#define AllowRunXFS (State_Bluetooth && Bluetooth_ConnectObject == BC_Type::BC_XFS)

/**
  * @brief  语音合成串口中断回调
  * @param  无
  * @retval 无
  */
static void XFS_USART_Callback()
{
    if(AllowRunXFS)
        xfs.ChipStatus = xfs.XFS_Serial->read();
}

/**
  * @brief  初始化语音合成
  * @param  无
  * @retval 无
  */
void Init_XFS()
{
    XFS_SERIAL.begin(115200);
    XFS_SERIAL.attachInterrupt(XFS_USART_Callback);
    xfs.SetReader(xfs.Reader_XiaoYan);
//  xfs.SetLanguage(xfs.Language_English);
    xfs.SetStyle(xfs.Style_Continue);
    xfs.SetArticulation(xfs.Articulation_Word);
}

/**
  * @brief  清空语音合成队列
  * @param  无
  * @retval 无
  */
void XFS_Clear()
{
    SpeakStr.flush();
}

/**
  * @brief  加入语音合成队列
  * @param  无
  * @retval 无
  */
void XFS_Speak(String str)
{
    if(!AllowRunXFS || !hc05.GetState())
        return;

    SpeakStr.write(str);
}

/**
  * @brief  加入语音合成队列，支持自定义格式化输出
  * @param  无
  * @retval 无
  */
int XFS_Speak(const char *__restrict __format, ...)
{
    if(!AllowRunXFS || !hc05.GetState())
        return 0;

    char printf_buff[100];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    SpeakStr.write(String(printf_buff));

    return ret_status;
}

/**
  * @brief  语音合成队列扫描任务
  * @param  无
  * @retval 无
  */
void Task_XFS_ListCheck()
{
    static bool IsInit = false;
    if(!AllowRunXFS)
        return;

    if(!IsInit)
    {
        Init_XFS(); //语音合成器初始化
        //XFS_Speak("系统已就绪"); //语音进入待合成队列
        IsInit = true;
    }

    /*当语音合成空闲，并且队列中有待合成的语音时*/
    if(xfs.ChipStatus == xfs.ChipStatus_Idle && SpeakStr.available())
        xfs.StartSynthesis(SpeakStr.read());//合成语音
}
