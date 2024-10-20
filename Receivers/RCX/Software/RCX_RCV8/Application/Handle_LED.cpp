#include "FileGroup.h"
#include "ComPrivate.h"

void Task_InfoLED(MTM::Task* task)
{
    if (RCX::GetRxConnected())
    {
        digitalWrite_HIGH(LED_Pin);
    }
    else if (RCX::GetRxPackErrorCode())
    {
        task->setPeriod(100);
        togglePin(LED_Pin);
    }
    else
    {
        task->setPeriod(200);
        togglePin(LED_Pin);
    }
}
