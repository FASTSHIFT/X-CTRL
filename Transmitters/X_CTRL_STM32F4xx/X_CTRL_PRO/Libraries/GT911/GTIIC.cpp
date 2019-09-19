#include "gtiic.h"
#include "delay.h"

#define GT_SDA_IN()  GT_SetSdaMode(0)
#define GT_SDA_OUT() GT_SetSdaMode(1)

void GT_SetSdaMode(uint8_t now)
{
    static uint8_t last = !now;
    if(now != last)
    {
        now ? pinMode(GT_SDA_Pin, OUTPUT) : pinMode(GT_SDA_Pin, INPUT);
        last = now;
    }
}

/*****************************************************************************
 * @name       :void GT_Delay(void)
 * @date       :2019-06-20
 * @function   :Delay in controlling I2C speed
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT_Delay(void)
{
    delay_us(1);
}

/*****************************************************************************
 * @name       :void GT_IIC_Init(void)
 * @date       :2019-06-20
 * @function   :Capacitive touch chip IIC interface initialization
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT_IIC_Init(void)
{
    pinMode(GT_SDA_Pin, OUTPUT);
    pinMode(GT_SCL_Pin, OUTPUT);

    GT_IIC_SDA_SET;
    GT_IIC_SCL_SET;
}

/*****************************************************************************
 * @name       :void GT_IIC_Start(void)
 * @date       :2019-06-20
 * @function   :Generating IIC starting signal
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT_IIC_Start(void)
{
    GT_SDA_OUT();     //sda线输出
    GT_IIC_SDA_SET;
    GT_IIC_SCL_SET;
    GT_Delay();
    GT_IIC_SDA_CLR; //START:when CLK is high,DATA change form high to low
    GT_Delay();
    GT_IIC_SCL_CLR; //钳住I2C总线，准备发送或接收数据
}

/*****************************************************************************
 * @name       :void GT_IIC_Stop(void)
 * @date       :2019-06-20
 * @function   :Generating IIC stop signal
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT_IIC_Stop(void)
{
    GT_SDA_OUT();//sda线输出
    GT_IIC_SCL_CLR;
    GT_IIC_SDA_CLR;
    GT_Delay();
    GT_IIC_SCL_SET;
    GT_Delay();
    GT_IIC_SDA_SET; //STOP:when CLK is high DATA change form low to high
}

/*****************************************************************************
 * @name       :u8 GT_IIC_Wait_Ack(void)
 * @date       :2019-06-20
 * @function   :Wait for the response signal
 * @parameters :None
 * @retvalue   :0-receive response signal successfully
                                1-receive response signal unsuccessfully
******************************************************************************/
u8 GT_IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    GT_SDA_IN();      //SDA设置为输入
    GT_IIC_SDA_SET;
    delay_us(1);
    GT_IIC_SCL_SET;
    delay_us(1);
    while(GT_READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            GT_IIC_Stop();
            return 1;
        }
    }
    GT_IIC_SCL_CLR; //时钟输出0
    return 0;
}

/*****************************************************************************
 * @name       :void GT_IIC_Ack(void)
 * @date       :2019-06-20
 * @function   :Generate ACK response signal
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT_IIC_Ack(void)
{
    GT_IIC_SCL_CLR;
    GT_SDA_OUT();
    GT_IIC_SDA_CLR;
    GT_Delay();
    GT_IIC_SCL_SET;
    GT_Delay();
    GT_IIC_SCL_CLR;
}

/*****************************************************************************
 * @name       :void GT_IIC_NAck(void)
 * @date       :2019-06-20
 * @function   :Don't generate ACK response signal
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT_IIC_NAck(void)
{
    GT_IIC_SCL_CLR;
    GT_SDA_OUT();
    GT_IIC_SDA_SET;
    GT_Delay();
    GT_IIC_SCL_SET;
    GT_Delay();
    GT_IIC_SCL_CLR;
}

/*****************************************************************************
 * @name       :void GT_IIC_Send_Byte(u8 txd)
 * @date       :2019-06-20
 * @function   :send a byte data by IIC bus
 * @parameters :txd:Data to be sent
 * @retvalue   :None
******************************************************************************/
void GT_IIC_Send_Byte(u8 txd)
{
    u8 t;
    GT_SDA_OUT();
    GT_IIC_SCL_CLR; //拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {
        (txd & 0x80) >> 7 ? GT_IIC_SDA_SET : GT_IIC_SDA_CLR;
        txd <<= 1;
        GT_IIC_SCL_SET;
        GT_Delay();
        GT_IIC_SCL_CLR;
        GT_Delay();
    }
    GT_IIC_Wait_Ack();
}

/*****************************************************************************
 * @name       :u8 GT_IIC_Read_Byte(unsigned char ack)
 * @date       :2019-06-20
 * @function   :read a byte data by IIC bus
 * @parameters :ack:0-send nACK
                                      1-send ACK
 * @retvalue   :Data to be read
******************************************************************************/
u8 GT_IIC_Read_Byte(unsigned char ack)
{
    u8 i, receive = 0;
    GT_SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++ )
    {
        GT_IIC_SCL_CLR;
        delay_us(5);
        GT_IIC_SCL_SET;
        receive <<= 1;
        if(GT_READ_SDA)receive++;
    }
    if (!ack)GT_IIC_NAck();//发送nACK
    else GT_IIC_Ack(); //发送ACK
    return receive;
}




























