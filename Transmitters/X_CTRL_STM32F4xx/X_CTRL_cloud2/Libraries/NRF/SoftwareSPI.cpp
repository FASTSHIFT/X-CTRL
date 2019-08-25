#include "SoftwareSPI.h"
#include "arduino.h"

#define SoftwareSPI_ClockDelay	for(volatile int _spi_delay_ = 0; _spi_delay_ < 30; _spi_delay_++);

SoftwareSPI::SoftwareSPI(SOFTWARESPI_PORT_TYPEDEF mosi, SOFTWARESPI_PORT_TYPEDEF miso, SOFTWARESPI_PORT_TYPEDEF sck, SOFTWARESPI_PORT_TYPEDEF csn)
{
	MOSI = mosi;
	MISO = miso;
	SCK = sck;
	CSN = csn;
}
void SoftwareSPI::Init()
{
  this->ChipRelease();
}
void SoftwareSPI::ChipSetect()
{
	SoftwareSPI_ClockDelay;
  SoftwareSPI_ResetPin(CSN);
	SoftwareSPI_ClockDelay;
}
void SoftwareSPI::ChipRelease()
{
	SoftwareSPI_ClockDelay;
  SoftwareSPI_SetPin(CSN);
	SoftwareSPI_ClockDelay;
}
void SoftwareSPI::WriteByte(char dat)
{
  char tmpDat = 0;
  SoftwareSPI_ResetPin(SCK);
	SoftwareSPI_ClockDelay;
  for(int i = 0; i < 8; i++)
  {
    if(dat & 0x80)
    {
			SoftwareSPI_SetPin(MOSI);
    }
    else
    {
			SoftwareSPI_ResetPin(MOSI);
    }
    dat <<= 1;
    tmpDat <<= 1;
		SoftwareSPI_ClockDelay;
		SoftwareSPI_SetPin(SCK);
    tmpDat += (SoftwareSPI_GetPin(MISO) != 0);
		SoftwareSPI_ResetPin(SCK);
		SoftwareSPI_ClockDelay;
  }
  Data = tmpDat;
}
char SoftwareSPI::ReadByte()
{
  return Data;
}

