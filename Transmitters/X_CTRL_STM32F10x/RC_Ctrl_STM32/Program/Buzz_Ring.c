#include "FileGroup.h"

void RING(unsigned char chs)
{
  if(Silent_Mode)
  {
    goto skip_ring;
  }
  switch (chs)
  {
    case 1:
      tone(BUZZ, 100);
      delay(100);
      tone(BUZZ, 100);
      delay(100);
      break;

    case 2:
      tone(BUZZ, 1046);
      delay(80);
      tone(BUZZ, 1175);
      delay(80);
      tone(BUZZ, 1318);
      delay(80);
      break;

    case 3:
      tone(BUZZ, 523);
      delay(60);
      tone(BUZZ, 587);
      delay(60);
      tone(BUZZ, 659);
      delay(60);
      tone(BUZZ, 698);
      delay(60);
      tone(BUZZ, 523);
      delay(60);
      tone(BUZZ, 587);
      delay(60);
      tone(BUZZ, 659);
      delay(60);
      tone(BUZZ, 698);
      delay(60);
      break;

    case 4:
      tone(BUZZ, 523);//1
      delay(80);
      tone(BUZZ, 880);//6
      delay(80);
      tone(BUZZ, 659);//3
      delay(80);
      break;

     case 5:
      tone(BUZZ, 1000);
      delay(20);
  }

skip_ring:
  ring_flag = chs;
}

void lcd_DCprint(u8 x, u8 y, char *str, unsigned int dly)
{
  u8 i,j = 0, strleth = strlen(str);
  while (i < strleth)
  {
    if (str[i] >= 'A' && str[i] <= 'z')
    {
      for (j = 'A'; j <= str[i]; j++)
      {
        lcd.setCursor(x, y);
        lcd.print(j);
        if(!Silent_Mode)tone(BUZZ, 7000, dly);
      }
    }
    else if (str[i] >= ' ' && str[i] <= '@')
    {
      for (j = ' '; j <= str[i]; j++)
      {
        lcd.setCursor(x, y);
        lcd.print(j);
        if(!Silent_Mode)tone(BUZZ, 7000, dly);
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