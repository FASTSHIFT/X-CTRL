#include "Arduino.h"

/**
  * @brief  配置引脚输入输出模式
  * @param  Pin: 引脚编号
			GPIO_Mode_x: 模式
  * @retval 无
  */
void pinMode(uint8_t Pin, uint8_t GPIO_Mode_x)
{
    if(IS_GPIO_MODE(GPIO_Mode_x))
    {
        GPIOx_Init(PIN_MAP[Pin].GPIOx, PIN_MAP[Pin].GPIO_Pin_x, (GPIOMode_TypeDef)GPIO_Mode_x, GPIO_Speed_50MHz);
    }
    else if(GPIO_Mode_x == PWM) PWM_Init(Pin, 1000, 20000);
}

/**
  * @brief  将数字HIGH(1)或LOW(0)值写入数字引脚
  * @param  Pin:引脚编号
			val: 写入值
  * @retval 无
  */
void digitalWrite(uint8_t Pin, uint8_t val)
{
    val ? digitalWrite_HIGH(Pin) : digitalWrite_LOW(Pin);
}

/**
  * @brief  读取引脚电平
  * @param  Pin: 引脚编号
  * @retval 引脚电平
  */
uint8_t digitalRead(uint8_t Pin)
{
    return(GPIO_READ(PIN_MAP[Pin].GPIOx, PIN_MAP[Pin].GPIO_Pin_x));
}

/**
  * @brief  将模拟值(PWM占空比)写入引脚
  * @param  Pin: 引脚编号
			val: PWM占空比
  * @retval PWM占空比
  */
uint16_t analogWrite(uint8_t Pin, uint16_t val)
{
    return (IS_PWM_PIN(Pin) ? pwmWrite(Pin, val) : 0);
}

/**
  * @brief  从指定的模拟引脚读取值
  * @param  Pin: 引脚编号
  * @retval ADC值：0~4095
  */
uint16_t analogRead(uint8_t Pin)
{
    return (IS_ADC_PIN(Pin) ? Get_ADC(PIN_MAP[Pin].ADCx, PIN_MAP[Pin].ADC_Channel) : 0);
}

/**
  * @brief  从指定的模拟引脚读取值(DMA方式)
  * @param  Pin: 引脚编号
  * @retval ADC值：0~4095
  */
uint16_t analogRead_DMA(uint8_t Pin)
{
    return (IS_ADC_PIN(Pin) ? Get_DMA_ADC(PIN_MAP[Pin].ADC_Channel) : 0);
}


/**
  * @brief  一次移出一个字节的数据，从最左端或最小(最右边)开始
  * @param  dataPin: 输出每个位的 pin
			clockPin: 将 dataPin 设置为正确值后要切换的 pin (int)
			bitOrder: MSBFIRST / LSBFIRST
			value: 要移出的数据(字节)
  * @retval ADC值：0~4095
  */
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value)
{
    uint8_t i;
    digitalWrite_LOW(clockPin);
    for (i = 0; i < 8; i++)
    {
        int bit = bitOrder == LSBFIRST ? i : (7 - i);
        digitalWrite(dataPin, (value >> bit) & 0x1);
        togglePin(clockPin);
        togglePin(clockPin);
    }
}

/**
  * @brief  一次将一个字节的数据移位，从最左端或最小 (最右边) 开始
  * @param  dataPin:  输入每个位的 pin
			clockPin: 要切换到的 pin 信号从dataPin读取
			bitOrder: MSBFIRST/LSBFIRST
  * @retval 读取的值 (字节)
  */
uint32_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint32_t bitOrder)
{
    uint8_t value = 0 ;
    uint8_t i ;
    for ( i = 0 ; i < 8 ; ++i )
    {
        digitalWrite_HIGH(clockPin) ;
        if (bitOrder == LSBFIRST )
        {
            value |= digitalRead(dataPin) << i ;
        }
        else
        {
            value |= digitalRead(dataPin) << (7 - i) ;
        }
        digitalWrite_LOW(clockPin);
    }

    return value ;
}

/**
  * @brief  将一个数字(整形)从一个范围重新映射到另一个区域
  * @param  x: 要映射的数字
			in_min: 值的当前范围的下界
			in_max: 值的当前范围的上界
			out_min: 值的目标范围的下界
			out_max: 值目标范围的上界
  * @retval 映射的值(long)
  */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
  * @brief  将一个数字(浮点型)从一个范围重新映射到另一个区域
  * @param  x: 要映射的数字
			in_min: 值的当前范围的下界
			in_max: 值的当前范围的上界
			out_min: 值的目标范围的下界
			out_max: 值目标范围的上界
  * @retval 映射的值(double)
  */
double fmap(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
  * @brief  在引脚上读取脉冲
  * @param  pin: 要读取脉冲的引脚编号
			value: 脉冲类型:高或低
			timeout: 等待脉冲开始的微秒数
  * @retval 脉冲的长度(以微秒计)或0, 如果没有脉冲在超时之前开始
  */
uint32_t pulseIn(uint32_t Pin, uint32_t State, uint32_t Timeout)
{
    uint8_t bit = digitalRead(Pin);
    uint8_t stateMask = (State ? bit : 0);

    unsigned long start;
    unsigned long startMicros = micros();

    // wait for any previous pulse to end
    while ((digitalRead(Pin) & bit) == stateMask)
    {
        if (micros() - startMicros > Timeout)
            return 0;
    }

    // wait for the pulse to start
    while ((digitalRead(Pin) & bit) != stateMask)
    {
        if (micros() - startMicros > Timeout)
            return 0;
    }

    start = micros();
    // wait for the pulse to stop
    while ((digitalRead(Pin) & bit) == stateMask)
    {
        if (micros() - startMicros > Timeout)
            return 0;
    }
    return micros() - start;
}
