
#ifndef __SOFTWARESPI_H__
#define __SOFTWARESPI_H__

#include "arduino.h"

typedef uint32_t SOFTWARESPI_PORT_TYPEDEF;

#define SoftwareSPI_SetPin(n)				digitalWrite_HIGH(n)
#define SoftwareSPI_ResetPin(n)			digitalWrite_LOW(n)
#define SoftwareSPI_GetPin(n)				digitalRead_Fast(n)

/* 表示一个软件 SPI 接口的实例 */
class SoftwareSPI
{
private:
  char Data;						/* 写入的同时取出的数据 */
public:
  SOFTWARESPI_PORT_TYPEDEF MOSI;				/* MOSI 引脚的位掩码 */
  SOFTWARESPI_PORT_TYPEDEF MISO;
  SOFTWARESPI_PORT_TYPEDEF SCK;
  SOFTWARESPI_PORT_TYPEDEF CSN;
	SoftwareSPI(SOFTWARESPI_PORT_TYPEDEF mosi, SOFTWARESPI_PORT_TYPEDEF miso, SOFTWARESPI_PORT_TYPEDEF sck, SOFTWARESPI_PORT_TYPEDEF csn);
  void ChipSetect();					/* 选中该 SPI 设备 */
  void ChipRelease();					/* 释放该 SPI 设备 */
  void WriteByte(char dat);		/* 向该 SPI 设备写入一个字节 */
  char ReadByte();						/* 获取上一次写字节同时取出的字节 */
  void Init();								/* 初始化该 SPI 实例，初始化引脚并将引脚置为默认状态 */
};


#endif


