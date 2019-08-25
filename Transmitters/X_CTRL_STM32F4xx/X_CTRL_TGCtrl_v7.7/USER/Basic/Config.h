#ifndef __CONFIG_H
#define __CONFIG_H

#define ADC_MaxValue 4095
#define ADC_HalfValue (ADC_MaxValue/2)
//#define USE_Encoder

//*************GPIO**************//

#define NULL_PIN PA12

#define KEY_UP_Pin      PB4
#define KEY_DOWN_Pin    PB3
#define KEY_BACK_Pin    PC8
#define KEY_OK_Pin      PC9
#define KEY_A_Pin       PD2
#define KEY_B_Pin       PC12

#define JSL_X_Pin PC2
#define JSL_Y_Pin PC3
#define JSR_X_Pin PC0
#define JSR_Y_Pin PC1
#define ADL_Pin PA0
#define ADR_Pin PA1
#define BattSensor_Pin PA4
#define BattCharge_Pin PB9

#define JSL_X_ADC()         (ADC_MaxValue-analogRead_DMA(JSL_X_Pin))
#define JSL_Y_ADC()         analogRead_DMA(JSL_Y_Pin)
#define JSR_X_ADC()         (ADC_MaxValue-analogRead_DMA(JSR_X_Pin))
#define JSR_Y_ADC()         analogRead_DMA(JSR_Y_Pin)
#define ADL_ADC()           analogRead_DMA(ADL_Pin)
#define ADR_ADC()           analogRead_DMA(ADR_Pin)
#define BattSensor_ADC()    analogRead_DMA(BattSensor_Pin)
#define Temp_ADC()          ADC_DMA_GetValue(ADC_Channel_TempSensor)

#ifdef USE_Encoder
#define EncoderLED_Pin  NULL_PIN
#define EncoderKey_Pin  NULL_PIN
#define EncoderA_Pin    NULL_PIN
#define EncoderB_Pin    NULL_PIN
#endif

#define Buzz_Pin            PA11
#define Motor_Pin           0xFF
#define SPDT_Switch_Pin     PB11
#define HC165_OUT_Pin       PB12
#define HC165_CP_Pin        PB13
#define HC165_PL_Pin        PB14

#define IS_EnableCtrl()     digitalRead_Fast(SPDT_Switch_Pin)
#define IS_BattCharge()     !digitalRead_Fast(BattCharge_Pin)

#define NRF_MOSI_Pin PB15
#define NRF_MISO_Pin PB14
#define NRF_SCK_Pin PB13
#define NRF_CE_Pin PC6
#define NRF_CSN_Pin PC7
#define NRF_IRQ_Pin NULL_PIN
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define BT_PowerCtrl_Pin    -1
#define BT_State_Pin        PA8
#define BT_EN_Pin           -1

#define TFT_CS      NULL_PIN
#define TFT_RST     PC4
#define TFT_DC      PC5
#define TFT_MOSI    PA7
#define TFT_CLK     PA5
#define TFT_LED     PB0

/*电池最低电压2.75V，最高4.15V*/
#define BattEmptyVoltage 2.75f
#define BattFullVoltage 4.15f

/*主调度器定时器*/
#define TIM_ControlTask TIM2

/*蜂鸣器定时器*/
#define TIM_BuzzTone    TIM1

/*握手通信定时中断定时器*/
#define TIM_Handshake   TIM4

/*语音模块串口输出*/
#define XFS_SERIAL Serial2

/*HMI串口输出*/
#define HMI_SERIAL Serial2

/*Lua串口输出*/
#define Lua_SERIAL Serial2

/*调试信息串口输出*/
#define Debug_SERIAL Serial

/*蓝牙通信串口输出*/
#define Bluetooth_SERIAL Serial2

/*无操作时间超时设置(秒数)*/
#define NoOperationTimeout 10*60

#endif
