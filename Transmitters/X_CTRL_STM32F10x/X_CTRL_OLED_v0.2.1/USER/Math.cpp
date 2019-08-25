#include "FileGroup.h"

double fmap(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Limit(int *data, int MIN, int MAX)
{
    if (*data < MIN)*data = MIN;
    else if (*data > MAX)*data = MAX;
}

void fLimit(double *data, double MIN, double MAX)
{
    if (*data < MIN)*data = MIN;
    else if (*data > MAX)*data = MAX;
}
