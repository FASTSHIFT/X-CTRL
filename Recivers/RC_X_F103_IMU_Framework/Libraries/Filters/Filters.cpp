#include "filters.h"


CustomFilter::CustomFilter(float dt, int pole, const float *rates) : DigitalFilter(dt)
{
	dT = dt;
	Pole = pole + 1;
	Rates = rates;
	Data = new float[pole];
	
	for(int i = 0; i < pole; i++)
	{
		Data[pole] = 0;
	}
}

float CustomFilter::Next(float value)
{
	Data[CrtPos] = value;
	float val = 0;
	for(int i = Pole; i >= 0; i--)
	{
		val += Data[(CrtPos + Pole + i) % Pole] * Rates[Pole - i];
	}
	CrtPos = (CrtPos + 1) % Pole;
	return val;
}

PT1Filter::PT1Filter(float dt, float cutoff) : DigitalFilter(dt)
{
	IsFirst = true;
	dT = dt;
	float RC = 1 / (2 * 3.141592653 * cutoff);
	RCRate = dT / (RC + dT);
}
float PT1Filter::Next(float value)
{
	if(IsFirst)
	{
		IsFirst = false;
		return LastValue = value;
	}
	else
	{
		return LastValue = (LastValue + (value - LastValue) * RCRate);
	}
}

SlewFilter::SlewFilter(float dt, float dPerSecond) : DigitalFilter(dt)
{
	IsFirst = true;
	dT = dt;
	delta = dPerSecond * dt;
}
float SlewFilter::Next(float value)
{
	if(IsFirst)
	{
		IsFirst = false;
		return LastValue = value;
	}
	else
	{
		if(value - LastValue > delta)
		{
			return LastValue = (LastValue + delta);
		}
		else if(LastValue - value > delta)
		{
			return LastValue = (LastValue - delta);
		}
		else
		{
			return LastValue = value;
		}
	}
}

FIRFilter::FIRFilter(float dt, int cnt) : DigitalFilter(dt)
{
	IsFirst = true;
	dT = dt;
	Buffer = new float[cnt];
	CrtPos = 0;
	Total = 0;
	Count = cnt;
}
FIRFilter::FIRFilter(float dt, int cnt, float *buf) : DigitalFilter(dt)
{
	IsFirst = true;
	dT = dt;
	Buffer = buf;
	CrtPos = 0;
	Total = 0;
	Count = cnt;
}
float FIRFilter::Next(float value)
{
	if(IsFirst)
	{
		IsFirst = false;
		for(int i = 0; i < Count; i++)
		{
			Buffer[i] = value;
			Total += value;
		}
		return value;
	}
	else
	{
		Total -= Buffer[CrtPos = ((CrtPos + 1) % Count)];
		Total += (Buffer[CrtPos] = value);
		return Total / Count;
	}
}

