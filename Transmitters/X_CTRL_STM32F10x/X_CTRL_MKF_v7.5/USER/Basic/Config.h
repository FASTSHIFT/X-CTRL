#ifndef __CONFIG_H
#define __CONFIG_H

#define ADC_MaxValue 4095
//#define USE_Encoder

//*************GPIO**************//

//#define NULL_PIN PC13

//#define IS_KEY_PRESSED(key,func) \
//do{\
//   if(!digitalRead(key))\
//   {\
//       delay(10);\
//       if(!digitalRead(key))\
//           func;\
//   }\
//}while(0)

/*ADC*/
#define JSL_X_Pin PC0
#define JSL_Y_Pin PC1
#define JSR_X_Pin PC2
#define JSR_Y_Pin PC3
#define BattSensor_Pin PA3

#define BattCharge_Pin PA2
#define InfoLED_Pin PC13

#define JSL_X_ADC()         (ADC_MaxValue-Get_DMA_ADC(0))
#define JSL_Y_ADC()         (ADC_MaxValue-Get_DMA_ADC(1))
#define JSR_X_ADC()         (ADC_MaxValue-Get_DMA_ADC(3))
#define JSR_Y_ADC()         Get_DMA_ADC(2)
#define ADL_ADC()           ADC_MaxValue
#define ADR_ADC()           ADC_MaxValue
#define BattSensor_ADC()    Get_DMA_ADC(4)
#define Temp_ADC()			Get_DMA_ADC(5)

#ifdef USE_Encoder
#define EncoderLED_Pin  NULL_PIN
#define EncoderKey_Pin  NULL_PIN
#define EncoderA_Pin    NULL_PIN
#define EncoderB_Pin    NULL_PIN
#endif

#define Buzz_Pin            PA8

#define SPDT_Switch_Pin     PA0

#define KEY_Y1_Pin PB0
#define KEY_Y2_Pin PB1
#define KEY_Y3_Pin PB6
#define KEY_X1_Pin PB7
#define KEY_X2_Pin PB8
#define KEY_X3_Pin PB9

#define NRF_MOSI_Pin PA7
#define NRF_MISO_Pin PA6
#define NRF_SCK_Pin PA5
#define NRF_CE_Pin PA4
#define NRF_CSN_Pin PC4
#define NRF_IRQ_Pin PC5
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin    -1
#define BT_State_Pin        -1
#define BT_EN_Pin           -1

//#define SDIO_D0_Pin PC8
//#define SDIO_D1_Pin PC9
//#define SDIO_D2_Pin PC10
//#define SDIO_D3_Pin PC11
//#define SDIO_SCK_Pin PC12
//#define SDIO_CMD_Pin PD2

//#define SD_CS_Pin   SDIO_D3_Pin
//#define SD_SCK_Pin  SDIO_SCK_Pin
//#define SD_MOSI_Pin SDIO_CMD_Pin
//#define SD_MISO_Pin SDIO_D0_Pin
//#define SD_CD_Pin   NULL_PIN

#define TFT_CS      PB12
#define TFT_RST     -1
#define TFT_DC      PC6
#define TFT_MOSI	PB15
#define TFT_CLK		PB13
#define TFT_LED     PC7

/*电池最低电压2.75V，最高4.15V*/
#define BattEmptyVoltage 2.75f
#define BattFullVoltage 4.15f

/*主调度器定时器*/
#define TIM_ControlTask TIM2

/*蜂鸣器定时器*/
#define TIM_BuzzTone	TIM1

/*握手通信定时中断定时器*/
#define TIM_Handshake	TIM4

/*语音模块串口输出*/
#define XFS_SERIAL Serial3

///*HMI串口输出*/
//#define HMI_SERIAL Serial3

///*Lua串口输出*/
//#define Lua_SERIAL Serial

/*调试信息串口输出*/
#define Debug_SERIAL Serial

/*蓝牙通信串口输出*/
#define Bluetooth_SERIAL Serial3

#endif
