#ifndef __CONFIG_H
#define __CONFIG_H

#define ADC_MaxValue 4095
#define btLEFT btBACK
#define btRIGHT btOK

//*************GPIO**************//
#define DEFAULT_PIN PC0

#define JSL_X_Pin       PA7
#define JSL_Y_Pin       PA6
#define BattSensor_Pin  DEFAULT_PIN
#define JSR_X_Pin       PA3
#define JSR_Y_Pin       PA2
#define SPTT_Switch_Pin PA1
#define SPDT_Switch_Pin PA0

#define JSL_X_ADC()         analogRead_DMA(7)
#define JSL_Y_ADC()         analogRead_DMA(6)
#define JSR_X_ADC()         analogRead_DMA(3)
#define JSR_Y_ADC()         analogRead_DMA(2)
#define SPTT_ADC()          analogRead_DMA(1)
#define BattSensor_ADC()    analogRead_DMA(0)

#define EncoderLED_Pin DEFAULT_PIN
#define EncoderKey_Pin DEFAULT_PIN
#define EncoderA_Pin   DEFAULT_PIN
#define EncoderB_Pin   DEFAULT_PIN

#define Buzz_Pin PB0

#define Button_OK_Pin PB7
#define Button_UP_Pin PB5
#define Button_BACK_Pin PB4
#define Button_DOWN_Pin PB6
#define Button_R_Pin PA8
#define Button_L_Pin PB1

/**NRF24L01 引脚分布图(模块正面,俯视)
  *----------------+-------+
  *|               |       |
  *|        _______|_______|
  *|        |8:IRQ | 7:MISO|
  *|        |6:MOSI| 5:SCK |
  *|        |4:CSN | 3:CE  |
  *|        |2:VCC | 1:GND |
  *----------------+-------+
  */
#define NRF_IRQ_Pin -1
#define NRF_MOSI_Pin PA11
#define NRF_MISO_Pin PA12
#define NRF_SCK_Pin  PB3
#define NRF_CE_Pin   PA15
#define NRF_CSN_Pin  PB2

/*电池最低电压2.75V，最高4.15V*/
#define BattEmptyVoltage 2.75
#define BattFullVoltage 4.15

#endif
