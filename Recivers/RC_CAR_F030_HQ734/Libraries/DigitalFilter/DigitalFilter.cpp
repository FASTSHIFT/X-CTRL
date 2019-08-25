#include "DigitalFilter.h"

DigitalFilter::DigitalFilter(uint16_t _filterSamples)
{
	if(_filterSamples%2 == 0)_filterSamples++;
	filterSamples = _filterSamples;
	
	if(sensSmoothArray != NULL) delete sensSmoothArray;
	sensSmoothArray = new int[filterSamples];
	
	if(sorted != NULL) delete sorted;
	sorted = new int[filterSamples];
}

int DigitalFilter::getSmooth(int rawIn)
{
  int j, k, temp, top, bottom;
  long total;
  static int i;
  bool done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  for (j=0; j<filterSamples; j++)
	{     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting              
  while(done != 1)
	{        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++)
		{
      if (sorted[j] > sorted[j + 1])
			{     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1); 
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++)
	{
    total += sorted[j];  // total remaining indices
    k++; 
  }
  return total / k;    // divide by number of samples
}
