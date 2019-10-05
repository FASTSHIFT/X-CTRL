#include "FileGroup.h"
#include "Module.h"

uint16_t BrightnessGet()
{
    return timer_get_compare(PIN_MAP[TFT_LED_Pin].TIMx, PIN_MAP[TFT_LED_Pin].TimerChannel);
}

void BrightnessSet(uint16_t val)
{
    __ExecuteOnce(PWM_Init(TFT_LED_Pin, 1000, 20000));
    analogWrite(TFT_LED_Pin, val);
}
