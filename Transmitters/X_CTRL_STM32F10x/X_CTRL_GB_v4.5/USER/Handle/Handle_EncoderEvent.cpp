#include "FileGroup.h"
#include "GUI_Private.h"

#define FreqStart 2000

EncoderEvent ecd;

static void When_EncoderRotate()
{
    static uint16_t Freq = FreqStart;
    static uint32_t FreqReload_TimePoint;
    if(millis() > FreqReload_TimePoint)
    {
        Freq = FreqStart;
    }
    else
    {
        if(ecd > 0)
            Freq += 100;
        
        if(ecd < 0)
            if(Freq > 100)
                Freq -= 100;
    }

    FreqReload_TimePoint = millis() + 1000;
    BuzzTone(Freq, 5);
	
	page.PageEventTransmit(EVENT_EncoderRotate);
}

void Init_EncoderEvent()
{
//	pinMode(EncoderLED_Pin, PWM);
//  analogWrite(EncoderLED_Pin, 0);
    pinMode(EncoderA_Pin, INPUT);
    pinMode(EncoderB_Pin, INPUT);
    ecd.EventAttach_Rotate(When_EncoderRotate);
}

void EncoderMonitor()
{
    ecd.EventMonitor(digitalRead(EncoderA_Pin), digitalRead(EncoderB_Pin));
}
