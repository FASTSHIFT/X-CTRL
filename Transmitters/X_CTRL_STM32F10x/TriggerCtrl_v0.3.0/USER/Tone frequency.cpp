#include "FileGroup.h"

typedef enum{
	L1=262,
	L1h=277,
	L2=294,
	L2h=311,
	L3=330,
	L4=349,
	L4h=370,
	L5=392,
	L5h=415,
	L6=440,
	L6h=466,
	L7=494,
	M1=523,
	M1h=554,
	M2=587,
	M2h=622,
	M3=659,
	M4=698,
	M4h=740,
	M5=784,
	M5h=831,
	M6=880,
	M6h=932,
	M7=988,
	H1=1046,
	H1h=1109,
	H2=1175,
	H2h=1245,
	H3=1318,
	H4=1397,
	H4h=1480,
	H5=1568,
	H5h=1661,
	H6=1760,
	H6h=1865,
	H7=1976,
}Tone_TypeDef;

void ToneDisplay(Tone_TypeDef t)
{

}
void nokia_tune(int volume,float speed)
{
	//speed = 1.0 - speed;
    toneBlock_Volume(Buzz_Pin,H5,200*speed,volume);
	toneBlock_Volume(Buzz_Pin,H4,200*speed,volume);
	toneBlock_Volume(Buzz_Pin,M6,350*speed,volume);
	toneBlock_Volume(Buzz_Pin,M7,350*speed,volume);
	toneBlock_Volume(Buzz_Pin,H4,200*speed,volume);
	toneBlock_Volume(Buzz_Pin,H3,200*speed,volume); 
	toneBlock_Volume(Buzz_Pin,M4,350*speed,volume);
	toneBlock_Volume(Buzz_Pin,M5,350*speed,volume); 
	toneBlock_Volume(Buzz_Pin,H2,200*speed,volume);
	toneBlock_Volume(Buzz_Pin,H1,200*speed,volume);
	toneBlock_Volume(Buzz_Pin,M3,350*speed,volume);
	toneBlock_Volume(Buzz_Pin,M5,350*speed,volume);
	toneBlock_Volume(Buzz_Pin,H1,500*speed,volume);
}
