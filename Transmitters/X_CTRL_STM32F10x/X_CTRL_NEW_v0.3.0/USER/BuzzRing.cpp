#include "FileGroup.h"

uint8_t Ring_Last;
bool SilentMode = OFF;

void BuzzRing(uint8_t chs)
{
    if(SilentMode)return;

    switch (chs)
    {
    case 1:
        toneBlock(Buzz_Pin, 100, 100);
        delay(100);
        toneBlock(Buzz_Pin, 100, 100);
        break;

    case 2:
        toneBlock(Buzz_Pin, 1046, 80);//80
        toneBlock(Buzz_Pin, 1175, 80);
        toneBlock(Buzz_Pin, 1318, 80);
        break;

    case 3:
        toneBlock(Buzz_Pin, 523, 60);
        toneBlock(Buzz_Pin, 587, 60);
        toneBlock(Buzz_Pin, 659, 60);
        toneBlock(Buzz_Pin, 698, 60);
        toneBlock(Buzz_Pin, 523, 60);
        toneBlock(Buzz_Pin, 587, 60);
        toneBlock(Buzz_Pin, 659, 60);
        toneBlock(Buzz_Pin, 698, 60);
        break;

    case 4:
        toneBlock(Buzz_Pin, 523, 80);//1
        toneBlock(Buzz_Pin, 880, 80);//6
        toneBlock(Buzz_Pin, 659, 80);//3
        break;
    }
}
