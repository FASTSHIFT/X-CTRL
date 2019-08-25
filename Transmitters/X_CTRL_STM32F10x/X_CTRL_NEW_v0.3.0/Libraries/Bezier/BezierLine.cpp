#include "bezier.h"

BezierLine::BezierLine()
{
	From = 0;
	To = 1;
	MinStep = 0;
	MaxStep = 100;
	ControlP2_X = ControlP1_X = 0.5;
	ControlP1_Y = 0.2;
	ControlP2_Y = 0.8;
}
float BezierLine::GetFromPercent(float k)
{
	float p01y,p12y,p23y;
	float p0112y,p1223y;
	float p01121223y;
	p01y = ControlP1_Y * k;
	p12y = ControlP1_Y - ControlP1_Y * k + ControlP2_Y * k;
	p23y = ControlP2_Y - ControlP2_Y * k + 1 * k;
	p0112y = p01y - p01y * k + p12y * k;
	p1223y = p12y - p12y * k + p23y * k;
	p01121223y = p0112y - p0112y * k + p1223y * k;
	return From - From * p01121223y + To * p01121223y ;
}
float BezierLine::Get(float step)
{
	if(MaxStep != MinStep)
	{
		return GetFromPercent((step - MinStep) / (MaxStep - MinStep));
	}
	return 0;
}
