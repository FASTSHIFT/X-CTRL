//Designed By _VIFEXTech
//2017.8.16  V_1.0
//12.5       V_1.1 增加初始化函数，以及地址设置函数
//2018.6.16  V_1.2 支持双向半双工数据回传
//6.18       V_1.3 加入模式设定合法性检测，支持使用指针获取模块地址
//6.19       V_1.4 支持硬件SPI，更高效的IO操作
//8.17       V_1.5 寄存器级别优化
//9.6		 V_1.6 优化SPI
//9.7		 V_1.7 将GPIO指针初始化放入构造函数
//9.21		 V_1.8 增加 IsDetect() 判断是否检测到NRF

#ifndef __NRF_H
#define __NRF_H

#include "Arduino.h"

//#define NRF_SPI_OBJECT SPI_2

#ifdef NRF_SPI_OBJECT
#include "SPI.h"
#define SPI_Transfer(x) NRF_SPI_OBJECT.transfer(x)
#endif

#if defined (__AVR__)
#ifdef NRF_SPI_OBJECT
#define NRF_SPI_OBJECT() {NRF_SPI_OBJECT.begin(),NRF_SPI_OBJECT.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));}
#endif
typedef volatile uint8_t PortReg_t;
typedef uint8_t PortMask_t;
#define USE_FAST_IO
#elif defined(__STM32__)
#ifdef NRF_SPI_OBJECT
#define NRF_SPI_BEGIN() {NRF_SPI_OBJECT.begin(SPISettings(10000000, MSBFIRST, SPI_MODE0));}
#endif
typedef volatile uint32_t PortReg_t;
typedef uint16_t PortMask_t;
#define USE_FAST_IO
#endif


#ifdef USE_FAST_IO
#define FAST_IO_HIGH(port,mask) (*port|=mask)
#define FAST_IO_LOW(port,mask)  (*port&=~mask)
#define FAST_IO_READ(port,mask) ((*port&mask)!=0)

#define NRF_MISO_READ() FAST_IO_READ(misoport,misopinmask)
#define NRF_MOSI_HIGH()	FAST_IO_HIGH(mosiport,mosipinmask)
#define NRF_MOSI_LOW() 	FAST_IO_LOW(mosiport,mosipinmask)
#define NRF_SCK_HIGH()	FAST_IO_HIGH(sckport,sckpinmask)
#define NRF_SCK_LOW() 	FAST_IO_LOW(sckport,sckpinmask)
#define NRF_CE_HIGH() 	FAST_IO_HIGH(ceport,cepinmask)
#define NRF_CE_LOW() 	FAST_IO_LOW(ceport,cepinmask)
#define NRF_CSN_HIGH() 	FAST_IO_HIGH(csnport,csnpinmask)
#define NRF_CSN_LOW() 	FAST_IO_LOW(csnport,csnpinmask)
#else
#define NRF_MISO_READ() digitalRead(_MISO)
#define NRF_MOSI_HIGH()	digitalWrite(_MOSI,1)
#define NRF_MOSI_LOW() 	digitalWrite(_MOSI,0)
#define NRF_SCK_HIGH()	digitalWrite(_SCK,1)
#define NRF_SCK_LOW() 	digitalWrite(_SCK,0)
#define NRF_CE_HIGH() 	digitalWrite(_CE,1)
#define NRF_CE_LOW() 	digitalWrite(_CE,0)
#define NRF_CSN_HIGH() 	digitalWrite(_CSN,1)
#define NRF_CSN_LOW() 	digitalWrite(_CSN,0)
#endif

//*************预指令**************//
#define NRF_READ_REG 0x00
#define NRF_WRITE_REG 0x20
#define RD_RX_PLOAD 0x61
#define WR_TX_PLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define NOP 0xFF
//***********中断标志位************//
#define RX_DR 0x40
#define TX_DS 0x20
#define MAX_RT 0x10
//**************寄存器*************//
#define CONFIG 0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_AW 0x03
#define SETUP_RETR 0x04
#define RF_CH 0x05
#define RF_SETUP 0x06
#define STATUS_ 0x07
#define OBSERVE_TX 0x08
#define CD 0x09
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17

typedef enum
{
    RX_MODE,
    TX_MODE,
    TXRX_MODE,
    RXTX_MODE
} NRF_Mode_TypeDef;

//*************数据宽度**************//
#define ADR_WIDTH 5

//8:IRQ 	7:MISO
//6:MOSI 	5:SCK
//4:CSN 	3:CE
//2:VCC		1:GND

class NRF
{
public:
    NRF(uint8_t _mosi, uint8_t _miso, uint8_t _sck, uint8_t _ce, uint8_t _csn);
#ifdef NRF_SPI_OBJECT
    NRF(uint8_t _ce, uint8_t _csn);
#endif
    void SetAddress(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4);
    uint8_t GetAddress(uint8_t addr);
    void init(NRF_Mode_TypeDef mode, uint8_t RX_length);
    void init(NRF_Mode_TypeDef mode, uint8_t *buff, uint8_t TX_length);
    void init(NRF_Mode_TypeDef mode, uint8_t *buff, uint8_t TX_length, uint8_t RX_length);
	bool IsDetect();
    uint8_t Status();
    uint8_t Tran(uint8_t *txbuff);
    uint8_t Recv(uint8_t *rxbuff);
    uint8_t TranRecv(uint8_t *txbuff, uint8_t *rxbuff);
    uint8_t RecvTran(uint8_t *rxbuff, uint8_t *txbuff);

private:
    uint8_t SPI_RW(uint8_t Data);
    uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value);
    uint8_t SPI_Read(uint8_t reg);
    uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);
    uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);
    void TX_Mode(uint8_t *txbuff);
    void RX_Mode();
    bool hwSPI;

    uint8_t ADDRESS[ADR_WIDTH];
    uint8_t TX_PLOAD_WIDTH;
    uint8_t RX_PLOAD_WIDTH;
    uint8_t _MOSI;
    uint8_t _MISO;
    uint8_t _SCK;
    uint8_t _CE;
    uint8_t _CSN;
#if defined(USE_FAST_IO)
    PortReg_t *mosiport, *misoport, *sckport, *ceport, *csnport;
    PortMask_t mosipinmask, misopinmask, sckpinmask, cepinmask, csnpinmask;
#endif
};

#endif
