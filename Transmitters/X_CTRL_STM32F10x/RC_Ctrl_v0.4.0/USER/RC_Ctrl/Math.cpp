#include "FileGroup.h"

/*void Limit(int16_t *data, int16_t MIN, int16_t MAX)
{
  if (*data < MIN)*data = MIN;
  else if (*data > MAX)*data = MAX;
}

void fLimit(double *data, double MIN, double MAX)
{
  if (*data < MIN)*data = MIN;
  else if (*data > MAX)*data = MAX;
}*/


double PowMap(double value, double oriStart, double oriEnd, double st, double end, double stK, double endK)
{
    double StartValue;
    double EndValue;
    double StartK;
    double EndK;
    double percent = (value - oriStart) / (oriEnd - oriStart);
    StartValue = st;
    EndValue = end;
    StartK = stK;
    EndK = endK;
    double x1 = log(StartK);
    double x2 = log(EndK);
    double k = (pow(EULER, x1 - x1 * percent + x2 * percent) - StartK) / (EndK - StartK);
    return StartValue + EndValue * k - StartValue * k;
}

