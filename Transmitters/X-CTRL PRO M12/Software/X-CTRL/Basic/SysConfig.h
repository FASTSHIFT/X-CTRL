#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#define XC_NAME             "X-CTRL"
#define XC_HARDWARE_VERSION "v1.0"
#define XC_SOFTWARE_VERSION "v1.5"

#define XC_CPU_NAME                 "STM32F411CEU6"
#define XC_CPU_RATE_MHZ             (100)
#define XC_ROM_SIZE                 (512 * 1024)
#define XC_RAM_SIZE                 (128 * 1024)

#define XC_BATTERY_NAME             "Li-ion"
#define XC_BATTERY_VOLTAGE          3700
#define XC_BATTERY_CAPACITY_MAH     4000
#define XC_BATTERY_CURRENT_MAX      2000
#define XC_BATTERY_VOLTAGE_MAX      4200
#define XC_BATTERY_VOLTAGE_MIN      3000

/*NULL*/
#define NULL_PIN    PD0

/*Joystick*/
#define JSL_X_Pin PA2
#define JSL_Y_Pin PA3
#define JSR_X_Pin PA0
#define JSR_Y_Pin PA1
#define JS_ADC_MAX          4095
#define JSL_X_ADC()         analogRead_DMA(JSL_X_Pin)
#define JSL_Y_ADC()         analogRead_DMA(JSL_Y_Pin)
#define JSR_X_ADC()         analogRead_DMA(JSR_X_Pin)
#define JSR_Y_ADC()         analogRead_DMA(JSR_Y_Pin)

/*LCD ST7789 IPS*/
#define LCD_SCL_Pin PA5
#define LCD_SDA_Pin PA7
#define LCD_CS_Pin  PA4
#define LCD_DC_Pin  PA6
#define LCD_RST_Pin PB2
#define LCD_BLK_Pin PB10/* TIM2 CH3 */

/*LED*/
#define LED_Pin     PA8/* TIM1 CH1 */

/*Charger*/
#define CHG_KEY_Pin PA15

/*NRF24L01*/
#define NRF_MOSI_Pin PB6
#define NRF_MISO_Pin PB7
#define NRF_SCK_Pin  PB5
#define NRF_CE_Pin   PB3
#define NRF_CSN_Pin  PB4
#define NRF_IRQ_Pin  -1

//#define NRF_MOSI_Pin PA7
//#define NRF_MISO_Pin PB2
//#define NRF_SCK_Pin  PA6
//#define NRF_CE_Pin   PA4
//#define NRF_CSN_Pin  PA5
//#define NRF_IRQ_Pin  PB10

/*SOFT I2C*/
#define SOFT_SCL_Pin PB8
#define SOFT_SDA_Pin PB9

/*MotorLRA*/
#define MotorLRA_IN1_Pin PB1/* TIM3 CH4 */
#define MotorLRA_IN2_Pin PB0/* TIM3 CH3 */

/*MotorERM*/
#define MotorERM_Pin        PB12

/*74HC165*/
#define HC165_OUT_Pin       PB15
#define HC165_CP_Pin        PB14
#define HC165_PL_Pin        PB13

/*Bluetooth*/
#define BT_Power_Pin        PC14
#define BT_State_Pin        PC13
#define BT_EN_Pin           PC15
#define BT_Serial           Serial6

/*TIM*/
#define TIM_MTM_MAIN        TIM5
#define TIM_MOTOR_TONE      TIM10
#define TIM_BIG_MOTOR       TIM9
//#define TIM_OS_RUNTIME      TIM5

#endif
