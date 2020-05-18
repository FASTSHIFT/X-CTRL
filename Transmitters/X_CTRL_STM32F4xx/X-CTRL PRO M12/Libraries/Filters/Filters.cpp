#include "filters.h"
#include "math.h"

BiquadFilter::BiquadFilter(float dt, float cutoff, float qualityFactor = BiquadFilter_DefaultQualityFactor) : DigitalFilter(dt)
{
    float omega = 2.0f * FILTER_M_PI * cutoff * dt;
    float sn = sin(omega);
    float cs = cos(omega);
    float alpha = sn / (2.0f * qualityFactor);
    // 2nd order Butterworth (with Q=1/sqrt(2)) / Butterworth biquad section with Q
    // described in http://www.ti.com/lit/an/slaa447/slaa447.pdf
    b0 = (1 - cs) * 0.5f;
    b1 = 1 - cs;
    b2 = (1 - cs) * 0.5f;
    a0 = 1 + alpha;
    a1 = -2 * cs;
    a2 = 1 - alpha;
    this->b0 = b0 / a0;
    this->b1 = b1 / a0;
    this->b2 = b2 / a0;
    this->a1 = a1 / a0;
    this->a2 = a2 / a0;
    this->x1 = this->x2 = 0;
    this->y1 = this->y2 = 0;
}

float BiquadFilter::Next(float value)
{
    float result =
        this->b0 * value +
        this->b1 * this->x1 +
        this->b2 * this->x2 -
        this->a1 * this->y1 -
        this->a2 * this->y2;

    /* shift x1 to x2, input to x1 */
    this->x2 = this->x1;
    this->x1 = value;

    /* shift y1 to y2, result to y1 */
    this->y2 = this->y1;
    this->y1 = result;

    return result;
}

ButterworthFilter::ButterworthFilter(float dt, float s_1, float a_2_1, float b_2_1, float a_3_1) : DigitalFilter(dt)
{
    Rate_a_2_1 = a_2_1;
    Rate_b_2_1 = b_2_1;
    Rate_a_3_1 = a_3_1;
    Rate_s_1 = s_1;
    last1 = last2 = last3 = last4 = 0;
}

float ButterworthFilter::Next(float value)
{
    float s_1_out = value * Rate_s_1;
//  float out1 = last1 * Rate_b_2_1;
//  float out2 = last2;
//  float out3 = last3 * Rate_a_2_1;
//  float out4 = last4 * Rate_a_3_1;
//  float out = s_1_out + out1 + out2  - out3 - out4;
//  last2 = last1;
//  last1 = s_1_out;
//  last4 = last3;
//  last3 = out;
    volatile float sum_1_out = s_1_out + last1 * Rate_b_2_1;
    volatile float sum_2_out = sum_1_out + last2;
    volatile float sum_3_out = sum_2_out - last3 * Rate_a_2_1;
    volatile float sum_4_out = sum_3_out - last4 * Rate_a_3_1;
    last2 = last1;
    last1 = s_1_out;
    last4 = last3;
    last3 = sum_4_out;

    return sum_4_out;
}

ButterworthFilter2::ButterworthFilter2(float dt, float s_1, float a_2_1, float b_2_1, float a_3_1) : DigitalFilter(dt)
{
    Rate_a_2_1 = a_2_1;
    Rate_b_2_1 = b_2_1;
    Rate_a_3_1 = a_3_1;
    Rate_s_1 = s_1;
}

float ButterworthFilter2::Next(float value)
{
    static float last1 = 0;
    static float last2 = 0;

    float s_1_out = value * Rate_s_1;
    float intg1_out = s_1_out - last1 * Rate_a_2_1;
    float intg2_out = intg1_out - last2 * Rate_a_3_1;
    float intg3_out = intg2_out + last1 * Rate_b_2_1;
    float intg4_out = intg3_out + last2;

    last2 = last1;
    last1 = intg2_out;

    return intg4_out;
}

CustomFilter::CustomFilter(float dt, int pole, const float *rates) : DigitalFilter(dt)
{
    dT = dt;
    Pole = pole + 1;
    Rates = rates;
    Data = new float[Pole];

    for(int i = 0; i < Pole; i++)
    {
        Data[i] = 0;
    }
}

float CustomFilter::Next(float value)
{
    Data[CrtPos] = value;
    float val = 0;
    for(int i = Pole; i >= 1; i--)
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
    if(cutoff > 0.001)
    {
        float RC = 1 / (2 * 3.141592653 * cutoff);
        RCRate = dT / (RC + dT);
    }
    else
    {
        RCRate = 1;
    }
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

