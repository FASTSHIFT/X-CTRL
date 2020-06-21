#include "FileGroup.h"
#include "GUI_Private.h"

/*蜂鸣器起始频率2000Hz*/
#define FreqStart 2000

/*实例化旋转编码器对象*/
EncoderEvent ecd;

/*LED亮度目标值*/
static uint16_t BrightSet;

/*上一次旋转时间戳*/
static uint32_t LastRotate_TimePoint;

/**
  * @brief  旋转编码器LED控制任务
  * @param  无
  * @retval 无
  */
static void Task_EncoderLED()
{
    static uint16_t LED_Bright;
    if(millis() - LastRotate_TimePoint > 500)
        BrightSet = 0;

    if(LED_Bright > BrightSet)
        LED_Bright -= 10;
    else if(LED_Bright < BrightSet)
        LED_Bright += 20;

    analogWrite(EncoderLED_Pin, LED_Bright);
}

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
static void When_EncoderRotate()
{
    BrightSet = 300;                    //LED亮度目标值设定300(30%)
    
    static uint16_t Freq = FreqStart;   //蜂鸣器频率

    if(millis() - LastRotate_TimePoint > 1000)//是否超时
    {
        Freq = FreqStart;               //重载蜂鸣器频率
    }
    else
    {
        if(ecd > 0)                     //顺时针旋转
            Freq += 100;                //频率提升100Hz
        
        if(ecd < 0)                     //逆时针旋转
            if(Freq > 100)              //限幅最低100Hz
                Freq -= 100;            //频率降低100Hz
    }

    LastRotate_TimePoint = millis();    //记录时间戳
    BuzzTone(Freq, 5);                  //蜂鸣器输出
	
	page.PageEventTransmit(EVENT_EncoderRotate);//向页面调度器发送一个旋转编码器事件
}

/**
  * @brief  初始化旋转编码器事件
  * @param  无
  * @retval 无
  */
void Init_EncoderEvent()
{
    /*初始化引脚*/
    pinMode(EncoderLED_Pin, PWM);
    analogWrite(EncoderLED_Pin, 0);
    pinMode(EncoderA_Pin, INPUT);
    pinMode(EncoderB_Pin, INPUT);
    
    ecd.EventAttach_Rotate(When_EncoderRotate);//关联旋转编码器事件
}

/**
  * @brief  旋转编码器监事
  * @param  无
  * @retval 无
  */
void EncoderMonitor()
{
    ecd.EventMonitor(digitalRead(EncoderA_Pin), digitalRead(EncoderB_Pin));
    __IntervalExecute(Task_EncoderLED(), 30);//每30ms更新一次LED状态
}
