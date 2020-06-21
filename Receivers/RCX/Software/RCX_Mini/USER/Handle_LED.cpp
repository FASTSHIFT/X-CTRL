#include "FileGroup.h"
#include "ComPrivate.h"

void Task_InfoLED()
{
    if (RCX::GetRxConnected())
    {
        digitalWrite_HIGH(LED_Pin);
    }
    else if (RCX::GetRxPackErrorCode())
    {
        mtm.TaskSetIntervalTime(TP_InfoLED, 100);
        togglePin(LED_Pin);
    }
    else
    {
        mtm.TaskSetIntervalTime(TP_InfoLED, 200);
        togglePin(LED_Pin);
    }
}
