#ifndef __BEZIERLINE_H__
#define __BEZIERLINE_H__


class BezierLine
{
public:
	float From;
	float To;
	float MinStep;
	float MaxStep;
	float ControlP1_X;
	float ControlP1_Y;
	float ControlP2_X;
	float ControlP2_Y;
	BezierLine();
	float GetFromPercent(float k);
	float Get(float step);
};

#endif
