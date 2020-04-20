#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "BSP.h"

static void Brightness_AnimCallback(void * obj, int16_t brightness)
{
    Brightness_SetValue(brightness);
}

void Brightness_SetGradual(uint16_t target, uint16_t time)
{
    static lv_anim_t a;
    lv_obj_add_anim(
        NULL, &a,
        (lv_anim_exec_xcb_t)Brightness_AnimCallback,
        Brightness_GetValue(), target,
        time
    );
}

uint16_t Brightness_GetValue()
{
    return timer_get_compare(PIN_MAP[LCD_BLK_Pin].TIMx, PIN_MAP[LCD_BLK_Pin].TimerChannel);
}

void Brightness_SetValue(int16_t val)
{
    __ExecuteOnce(PWM_Init(LCD_BLK_Pin, 1000, 20000));
    __LimitValue(val, 0, 1000);
    analogWrite(LCD_BLK_Pin, val);
}
