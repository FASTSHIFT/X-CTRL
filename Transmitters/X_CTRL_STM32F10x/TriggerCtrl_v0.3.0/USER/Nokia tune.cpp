#include "FileGroup.h"
void nokia_tune(int volume)
{
    toneBlock_Volume(Buzz_Pin,H5,200,volume);
	toneBlock_Volume(Buzz_Pin,H4,200,volume);
	toneBlock_Volume(Buzz_Pin,M6,350,volume);
	toneBlock_Volume(Buzz_Pin,M7,350,volume);
	toneBlock_Volume(Buzz_Pin,H4,200,volume);
	toneBlock_Volume(Buzz_Pin,H3,200,volume); 
	toneBlock_Volume(Buzz_Pin,M4,350,volume);
	toneBlock_Volume(Buzz_Pin,M5,350,volume); 
	toneBlock_Volume(Buzz_Pin,H2,200,volume);
	toneBlock_Volume(Buzz_Pin,H1,200,volume);
	toneBlock_Volume(Buzz_Pin,M3,350,volume);
	toneBlock_Volume(Buzz_Pin,M5,350,volume);
	toneBlock_Volume(Buzz_Pin,H1,500,volume);
}
