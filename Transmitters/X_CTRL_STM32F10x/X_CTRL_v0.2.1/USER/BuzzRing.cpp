#include "FileGroup.h"

uint8_t Ring_Last;

void BuzzRing(uint8_t chs)
{
  if(!Silent_Mode)
  {
		SendData_Switch(DISABLE);
		switch (chs)
		{
			case 1:
				toneBlock(BUZZ_Pin, 100, 100);
				delay(100);
				toneBlock(BUZZ_Pin, 100, 100);
				break;

			case 2:
				toneBlock(BUZZ_Pin, 1046, 80);//80
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
		}
		
		if(CTRL_State == On)SendData_Switch(ENABLE);
	}
  Ring_Last = chs;
}
