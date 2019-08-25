#ifndef __CONFIG_H
#define __CONFIG_H

#define ADC_MaxValue 4095

//*************GPIO**************//
#define KEY_Y1_Pin PC9
#define KEY_Y2_Pin PC8
#define KEY_Y3_Pin PB15
#define KEY_X1_Pin PA12
#define KEY_X2_Pin PA11
#define KEY_X3_Pin PA8

#define IS_KEY_PRESSED(key,func) \
do{\
   if(key)\
   {\
       delay(10);\
       if(key)\
           func;\
   }\
}while(0)

#define JSL_X_Pin PA0
#define JSL_Y_Pin PA1
#define JSR_X_Pin PA2
#define JSR_Y_Pin PA3
#define BattSensor_Pin PB0

#define JSL_Y_ADC()         analogRead_DMA(0)
#define JSL_X_ADC()         analogRead_DMA(1)
#define JSR_Y_ADC()         analogRead_DMA(2)
#define JSR_X_ADC()         analogRead_DMA(3)
#define BattSensor_ADC()    analogRead_DMA(4)
#define ADL_ADC()           ADC_MaxValue
#define ADR_ADC()           ADC_MaxValue

#define EncoderKey_Pin  PC13
#define EncoderA_Pin    PB13
#define EncoderB_Pin    PB14

#define Buzz_Pin    PA15

#define NRF_MOSI_Pin PB3
#define NRF_MISO_Pin PC12
#define NRF_SCK_Pin  PC11
#define NRF_CE_Pin   PC10
#define NRF_CSN_Pin  PD2
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin    PB1
#define BT_State_Pin        PB2
#define BT_EN_Pin           PB12

#define SD_CS_Pin   PA4
#define SD_SCK_Pin  PA5
#define SD_MISO_Pin PA6
#define SD_MOSI_Pin PA7

#define TFT_PORT_START_Pin PC0
#define TFT_BLK_Pin  PB4
#define TFT_RST_Pin  PB5
#define TFT_CS_Pin   PB6
#define TFT_RS_Pin   PB7
#define TFT_RW_Pin   PB8
#define TFT_RD_Pin   PB9

/*电池最低电压2.75V，最高4.15V*/
#define BattEmptyVoltage 2.75f
#define BattFullVoltage 4.15f

#endif
