#ifndef __FILTERS_H__
#define __FILTERS_H__

#define FILTER_M_PI                         3.1415926535897932384626433832795f
#define BiquadFilter_DefaultQualityFactor   0.70710678118654752440084436210485f

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
/* code from Betaflight-master filter.c */
class BiquadFilter : public DigitalFilter
{
public :
    float b0;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;
    float x1;
    float x2;
    float y1;
    float y2;

    BiquadFilter(float dt, float cutoff, float qualityFactor);

    virtual float Next(float value);
};
class ButterworthFilter2 : public DigitalFilter
{
public :
    float Rate_s_1;
    float Rate_a_2_1;
    float Rate_b_2_1;
    float Rate_a_3_1;

    ButterworthFilter2(float dt, float s_1, float a_2_1, float b_2_1, float a_3_1);

    virtual float Next(float value);
};
class ButterworthFilter : public DigitalFilter
{
public :
    volatile float last1;
    volatile float last2;
    volatile float last3;
    volatile float last4;
    volatile float Rate_s_1;
    volatile float Rate_a_2_1;
    volatile float Rate_b_2_1;
    volatile float Rate_a_3_1;

    ButterworthFilter(float dt, float s_1, float a_2_1, float b_2_1, float a_3_1);

    virtual float Next(float value);
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





