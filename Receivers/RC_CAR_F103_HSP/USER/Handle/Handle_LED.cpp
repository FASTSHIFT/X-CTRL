#include "FileGroup.h"
#include "ComPrivate.h"

void Init_InfoLED()
{
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, LOW);
    delay(80);
    digitalWrite(LED_Pin, HIGH);
    delay(80);
    digitalWrite(LED_Pin, LOW);
    delay(80);
    digitalWrite(LED_Pin, HIGH);
    delay(80);
    digitalWrite(LED_Pin, LOW);
}

void Task_InfoLED()
{
	if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
		digitalWrite_HIGH(LED_Pin);
    }
    else if(ConnectState.IsConnect && !ConnectState.IsCorrect)
    {
        mtm.TaskList[TP_InfoLED].IntervalTime = 100;
		togglePin(LED_Pin);
    }
	else
	{
        mtm.TaskList[TP_InfoLED].IntervalTime = 200;
		togglePin(LED_Pin);
	}
}
