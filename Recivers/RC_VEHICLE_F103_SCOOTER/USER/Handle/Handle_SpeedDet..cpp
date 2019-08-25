#include "FileGroup.h"

uint32_t SpdDet_Cnt = 0;

static void SpeedDet_Callback()
{
	SpdDet_Cnt++;
}

void Init_SpeedDet()
{
	pinMode(SPD_DET_Pin, INPUT);
	attachInterrupt(SPD_DET_Pin, SpeedDet_Callback, CHANGE);
}


