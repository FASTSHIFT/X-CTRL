#include "FileGroup.h"

#define Freq_Start 2000

EncoderEvent ecd(EncoderA_Pin, EncoderB_Pin);

uint16_t Freq = Freq_Start;
uint32_t FreqReload_TimePoint;

void When_Clockwise()
{
    if(millis() > FreqReload_TimePoint)
    {
        Freq = Freq_Start;
    }
    else
    {
        Freq += 100;
    }

    FreqReload_TimePoint = millis() + 1000;
    if(!SilentMode) tone(Buzz_Pin, Freq, 5);
    Menu_Up();
}

void When_Anticlockwise()
{
    if(millis() > FreqReload_TimePoint)
    {
        Freq = Freq_Start;
    }
    else
    {
        if(Freq > 100)Freq -= 100;
    }

    FreqReload_TimePoint = millis() + 1000;
    if(!SilentMode) tone(Buzz_Pin, Freq, 5);
    Menu_Down();
}

void Init_EncoderEvent()
{
    ecd.Init();
    ecd.EventAttach_Clockwise(When_Clockwise);
    ecd.EventAttach_Anticlockwise(When_Anticlockwise);
}

void Encoder_Monitor()
{
    ecd.Read();
}
