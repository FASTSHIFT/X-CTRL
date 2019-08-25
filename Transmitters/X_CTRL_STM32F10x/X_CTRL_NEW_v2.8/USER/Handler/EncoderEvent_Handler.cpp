#include "FileGroup.h"
#include "PageManager.h"

#define FreqStart 2000

EncoderEvent ecd(EncoderA_Pin, EncoderB_Pin);

uint16_t Freq = FreqStart;
uint32_t FreqReload_TimePoint;

void When_Clockwise()
{
    if(millis() > FreqReload_TimePoint)
    {
        Freq = FreqStart;
    }
    else
    {
        Freq += 100;
    }

    FreqReload_TimePoint = millis() + 1000;
    BuzzTone(Freq, 5);
	
	pm.PageEncoderEvent();
}

void When_Anticlockwise()
{
    if(millis() > FreqReload_TimePoint)
    {
        Freq = FreqStart;
    }
    else
    {
        if(Freq > 100)Freq -= 100;
    }
    FreqReload_TimePoint = millis() + 1000;
	
    BuzzTone(Freq, 5);
	
	pm.PageEncoderEvent();
}

void Init_EncoderEvent()
{
    ecd.Init();
    ecd.EventAttach_Clockwise(When_Clockwise);
    ecd.EventAttach_Anticlockwise(When_Anticlockwise);
}

void EncoderMonitor()
{
    ecd.Read();
}
