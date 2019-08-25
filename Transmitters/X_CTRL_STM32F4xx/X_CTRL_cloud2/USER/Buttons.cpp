/* user included */
#include "filegroup.h"


#include "buttons.h"

#ifdef Buttons_ClearAfterRead
#define Buttons_EventRead()		Buttons_Clear()
#else
#define Buttons_EventRead()
#endif

#define Buttons_Limit(index)	index < Buttons_Count ? index : (Buttons_Count - 1)

uint8_t  Buttons_Down[Buttons_Count];
uint8_t  Buttons_Up[Buttons_Count];
uint8_t  Buttons_Click[Buttons_Count];
uint8_t  Buttons_LongPressed[Buttons_Count];

uint8_t  Buttons_Status[Buttons_Count];
uint8_t  Buttons_Last[Buttons_Count];

uint32_t Buttons_MultiClickTimes[Buttons_Count];
uint32_t Buttons_ReleaseCycle[Buttons_Count];

uint32_t Buttons_DownCycle[Buttons_Count];

void Buttons_Read()
{
	for(int i = 0; i < 9; i++)
	{
		Buttons_Status[i] = !!btMK[i];
	}
	Buttons_Status[9] = !!btEcd;
}

void Buttons_Clear()
{
	uint8_t i;
	for(i = 0; i < Buttons_Count; i++)
	{
		Buttons_Down[i] = 0;
		Buttons_Up[i] = 0;
		Buttons_Click[i] = 0;
		Buttons_LongPressed[i] = 0;
		Buttons_MultiClickTimes[i] = 0;
		Buttons_ReleaseCycle[i] = 0;
		Buttons_DownCycle[i] = 0;
	}
}

void Buttons_Init()
{
	uint8_t i;
	for(i = 0; i < Buttons_Count; i++)
	{
		Buttons_Status[i] = 0;
		Buttons_Last[i] = 0;
	}
	Buttons_Clear();
}

void Buttons_Update()
{
	uint8_t i;
	Buttons_Read();
	
	for(i = 0; i < Buttons_Count; i++)
	{
		/* handle DOWN Event / UP Event */
		if(Buttons_Status[i] && !Buttons_Last[i])
		{
			Buttons_Down[i] = 1;
			Buttons_MultiClickTimes[i] ++;
		}
		else if(!Buttons_Status[i] && Buttons_Last[i])
		{
			Buttons_Up[i] = 1;
		}
		
		/* handle MULTI-CLICK Event */
		if(Buttons_Status[i])
		{
			Buttons_ReleaseCycle[i] = Buttons_MultiClickCycle;
		}
		else
		{
			if(Buttons_ReleaseCycle[i])
			{
				Buttons_ReleaseCycle[i] --;
				if(!Buttons_ReleaseCycle[i])
				{
					Buttons_Click[i] = Buttons_MultiClickTimes[i];
					Buttons_MultiClickTimes[i] = 0;
				}
			}
		}
		
		/* update status */
		Buttons_Last[i] = Buttons_Status[i];
		
		/* handle LONG-PRESS Event */
		if(Buttons_Status[i])
		{
			if(Buttons_DownCycle[i] < Buttons_LongPressCycle)
			{
				Buttons_DownCycle[i] ++;
				if(Buttons_DownCycle[i] == Buttons_LongPressCycle)
				{
					Buttons_LongPressed[i] = 1;
				}
			}
		}
		else
		{
			Buttons_DownCycle[i] = 0;
		}
	}
}

/* ------------------------- Events ------------------------- */
uint8_t Buttons_IsClicked(uint8_t index)
{
	uint8_t ret = Buttons_Click[Buttons_Limit(index)];
	if(ret)
	{
		Buttons_EventRead();
	}
	return ret;
}
uint8_t Buttons_IsPressed(uint8_t index)
{
	uint8_t ret = Buttons_Down[Buttons_Limit(index)];
	if(ret)
	{
		Buttons_EventRead();
	}
	return ret;
}
uint8_t Buttons_IsReleased(uint8_t index)
{
	uint8_t ret = Buttons_Up[Buttons_Limit(index)];
	if(ret)
	{
		Buttons_EventRead();
	}
	return ret;
}
uint8_t Buttons_IsLongPressed(uint8_t index)
{
	uint8_t ret = Buttons_LongPressed[Buttons_Limit(index)];
	if(ret)
	{
		Buttons_EventRead();
	}
	return ret;
}

/* ------------------------- Status ------------------------- */
uint8_t Buttons_IsPressing(uint8_t index)
{
	return Buttons_Status[Buttons_Limit(index)];
}
uint8_t Buttons_IsLongPressing(uint8_t index)
{
	return Buttons_DownCycle[Buttons_Limit(index)] == Buttons_LongPressCycle;
}

