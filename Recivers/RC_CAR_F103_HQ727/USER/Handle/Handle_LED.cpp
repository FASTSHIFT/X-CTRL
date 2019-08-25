#include "FileGroup.h"
#include "ComPrivate.h"

void Task_InfoLED()
{
	if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
		digitalWrite_HIGH(LED_Pin);
    }
    else if(ConnectState.IsConnect && !ConnectState.IsCorrect)
    {
        mtm.TaskList[2].IntervalTime = 100;
		togglePin(LED_Pin);
    }
	else
	{
        mtm.TaskList[2].IntervalTime = 200;
		togglePin(LED_Pin);
	}
}
