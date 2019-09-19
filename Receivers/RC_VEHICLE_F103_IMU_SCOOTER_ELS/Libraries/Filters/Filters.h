#ifndef __FILTERS_H__
#define __FILTERS_H__

class DigitalFilter
{
public :
	float dT;
	DigitalFilter(float dt)
	{
		dT = dt;
	}
	virtual float Next(float value) = 0;
};

class CustomFilter : public DigitalFilter
{
public :
	int Pole;
	int CrtPos;
	float *Data;
	const float *Rates;

	CustomFilter(float dt, int pole, const float *rates);

	virtual float Next(float value);
};
class PT1Filter : public DigitalFilter
{
public:
	float RCRate;
	float LastValue;
	bool IsFirst;
		
	PT1Filter(float dt, float cutoff);
	
	virtual float Next(float value);
};

class SlewFilter : public DigitalFilter
{
public:
	bool IsFirst;
	float delta;
	float LastValue;
	SlewFilter(float dt, float dPerSecond);
	
	virtual float Next(float value);
};

class FIRFilter : public DigitalFilter
{
public:
	float *Buffer;
	int Count;
	int CrtPos;
	float Total;
	bool IsFirst;
	FIRFilter(float dt, int cnt);
	FIRFilter(float dt, int cnt, float *buf);
	virtual float Next(float value);
};






#endif





