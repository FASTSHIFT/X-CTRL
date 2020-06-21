#include "FileGroup.h"
#include "XFS.h"
#include <stdio.h>
#include <stdarg.h>
#include "FifoQueue.h"

#define AllowRunXFS (State_Bluetooth && Bluetooth_ConnectObject == BC_Type::BC_XFS)

static XFS5152CE xfs(&Serial);
static FifoQueue<String> SpeakStr(5);

static void XFS_USART_Callback()
{
    xfs.ChipStatus = USART_ReceiveData(USART1);
}

void Init_XFS()
{
    Serial.attachInterrupt(XFS_USART_Callback);
    xfs.SetReader(xfs.Reader_XiaoYan);
//  xfs.SetLanguage(xfs.Language_English);
    xfs.SetStyle(xfs.Style_Continue);
    xfs.SetArticulation(xfs.Articulation_Word);
}

void XFS_Clear()
{
    SpeakStr.flush();
}

void XFS_Speak(String str)
{
    if(!AllowRunXFS || !hc05.GetState())
        return;

    SpeakStr.write(str);
}

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

void Task_XFS_ListCheck()
{
    if(!AllowRunXFS)
        return;
    
    if(xfs.ChipStatus == xfs.ChipStatus_Idle && SpeakStr.available())
        xfs.StartSynthesis(SpeakStr.read());
}
