#include "FileGroup.h"

void BuzzRing(uint8_t chs)
{
    if (Silent_Mode)
    {
        goto Mark_SkipRing;
    }
    switch (chs)
    {
    case 1:
        toneBlock(BUZZ_Pin, 100, 100);
        delay(100);
        toneBlock(BUZZ_Pin, 100, 100);
        break;

    case 2:
        toneBlock(BUZZ_Pin, 1046, 80);
        toneBlock(BUZZ_Pin, 1175, 80);
        toneBlock(BUZZ_Pin, 1318, 80);
        break;

    case 3:
        toneBlock(BUZZ_Pin, 523, 60);
        toneBlock(BUZZ_Pin, 587, 60);
        toneBlock(BUZZ_Pin, 659, 60);
        toneBlock(BUZZ_Pin, 698, 60);
        toneBlock(BUZZ_Pin, 523, 60);
        toneBlock(BUZZ_Pin, 587, 60);
        toneBlock(BUZZ_Pin, 659, 60);
        toneBlock(BUZZ_Pin, 698, 60);
        break;

    case 4:
        toneBlock(BUZZ_Pin, 523, 80);//1
        toneBlock(BUZZ_Pin, 880, 80);//6
        toneBlock(BUZZ_Pin, 659, 80);//3
        break;

    case 5:
        toneBlock(BUZZ_Pin, 1000, 20);
        break;
    }

Mark_SkipRing:
    RingLast = chs;
}

void LCD_DynamicPrint(uint8_t x, uint8_t y, char* str, uint32_t delayTime_ms)
{
    uint8_t i = 0, strleth = strlen(str);
    while (i < strleth)
    {
        if (str[i] >= 'A' && str[i] <= 'z')
        {
            for (char j = 'A'; j <= str[i]; j++)
            {
                lcd.setCursor(x, y);
                lcd.print(j);
                if (!Silent_Mode)tone(BUZZ_Pin, 7000, delayTime_ms);
            }
        }
        else if (str[i] >= ' ' && str[i] <= '@')
        {
            for (char j = ' '; j <= str[i]; j++)
            {
                lcd.setCursor(x, y);
                lcd.print(j);
                if (!Silent_Mode)tone(BUZZ_Pin, 7000, delayTime_ms);
            }
        }
        else
        {
            lcd.setCursor(x, y);
            lcd.print(str[i]);
        }
        x++;
        i++;
    }
}

