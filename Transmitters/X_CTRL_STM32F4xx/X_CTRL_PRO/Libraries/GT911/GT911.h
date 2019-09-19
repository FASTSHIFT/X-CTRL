#ifndef __GT911_DRIVER_H
#define __GT911_DRIVER_H

#include "gtiic.h"
#include "touch.h"

#define INT_Pin Touch_INT_Pin

#define TOUCH_MAX 5

#define RST_OUT()   //pinMode(RST_Pin,OUTPUT)//{   GPIOF->CRH&=0XFFFF0FFF;GPIOF->CRH|=0X00003000;}     //set RSSET pin to output
#define INT_OUT()   //pinMode(INT_Pin,OUTPUT)//{   GPIOF->CRH&=0XFFFFF0FF;GPIOF->CRH|=0X00000300;}     //set RSSET pin to output
#define INT_IN()    //pinMode(INT_Pin,INPUT)//{   GPIOF->CRH&=0XFFFFFFFF;GPIOF->CRH|=0X00000400;}     //set RSSET pin to output

//RST--PF11
//INT--PF10
#define RST_CTRL_SET    //digitalWrite(RST_Pin,HIGH)//PFout(11)   //GT911 RESET pin out high or low
#define RST_CTRL_CLR    //digitalWrite(RST_Pin,LOW)
#define INT_CTRL_SET    digitalWrite_HIGH(INT_Pin)//GT911 INT pin out high or low
#define INT_CTRL_CLR    digitalWrite_LOW(INT_Pin)
#define INT_GET         digitalRead(INT_Pin)//Get GT911 INT pin status


#define GT9XX_IIC_RADDR 0xBB    //IIC read address
#define GT9XX_IIC_WADDR 0xBA    //IIC write address

#define GT9XX_READ_ADDR 0x814E  //touch point information
#define GT9XX_ID_ADDR 0x8140    //ID of touch IC


void GT911_int_sync(uint16_t ms);
void GT911_reset_guitar(uint8_t addr);
void GT911_gpio_init(void);
uint8_t GT9XX_WriteHandle (uint16_t addr);
uint8_t GT9XX_WriteData (uint16_t addr, uint8_t value);
uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value);
uint8_t GT911_Init(void);
uint8_t Touch_Get_Count(void);
uint8_t GT911_Scan(void);
void GT9xx_send_config(void);
void GT9xx_Eint_Init(void);


#endif
