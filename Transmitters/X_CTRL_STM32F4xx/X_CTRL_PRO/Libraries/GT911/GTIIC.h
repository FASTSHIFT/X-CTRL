#ifndef __GT_IIC_H
#define __GT_IIC_H

#include "Arduino.h"
#include "SysConfig.h"

#define GT_SDA_Pin Touch_SDA_Pin
#define GT_SCL_Pin Touch_SCL_Pin

//IO方向设置
//#define GT_SDA_IN()  pinMode(GT_SDA_Pin, INPUT)//{GPIOF->CRH&=0XFFFFFF0F;GPIOF->CRH|=8<<4*1;}
//#define GT_SDA_OUT() pinMode(GT_SDA_Pin, OUTPUT)//{GPIOF->CRH&=0XFFFFFF0F;GPIOF->CRH|=3<<4*1;}

//IO操作函数
#define GT_IIC_SCL_SET    digitalWrite_HIGH(GT_SCL_Pin)//SCL
#define GT_IIC_SCL_CLR    digitalWrite_LOW(GT_SCL_Pin)
#define GT_IIC_SDA_SET    digitalWrite_HIGH(GT_SDA_Pin)//SDA
#define GT_IIC_SDA_CLR    digitalWrite_LOW(GT_SDA_Pin)
#define GT_READ_SDA       digitalRead_FAST(GT_SDA_Pin)//输入SDA 

//IIC所有操作函数
void GT_IIC_Init(void);                 //初始化IIC的IO口
void GT_IIC_Start(void);                //发送IIC开始信号
void GT_IIC_Stop(void);                 //发送IIC停止信号
void GT_IIC_Send_Byte(uint8_t txd);          //IIC发送一个字节
uint8_t GT_IIC_Read_Byte(uint8_t ack); //IIC读取一个字节
uint8_t GT_IIC_Wait_Ack(void);               //IIC等待ACK信号
void GT_IIC_Ack(void);                  //IIC发送ACK信号
void GT_IIC_NAck(void);                 //IIC不发送ACK信号

#endif







