#ifndef __DIGITALFILTER_H
#define __DIGITALFILTER_H

#include "Arduino.h"

class DigitalFilter
{
	public:
		DigitalFilter(uint16_t _filterSamples);
		int getSmooth(int rawIn);
	private:
		int* sensSmoothArray;
		int* sorted;
		uint16_t filterSamples;
};

#endif
