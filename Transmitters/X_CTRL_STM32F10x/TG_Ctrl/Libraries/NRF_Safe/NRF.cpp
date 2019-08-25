#include "nrf.h"

NRFCommands_TypeDef NRFCommands;
NRFRegesters_TypeDef NRFRegesters;
NRFModes_TypeDef NRFModes;
NRFBaudRates_TypeDef NRFBaudRates;
NRFCRCEncodingSchemes_TypeDef NRFCRCEncodingSchemes;
NRFParameters_TypeDef NRFDefaultParameters;

NRF::NRF()
{
	
}
NRF::NRF(SoftwareSPI *spi)
{
	OccuppiedSPI = spi;
}
NRF::NRF(SoftwareSPI *spi, int *nrf_ce)
{
	OccuppiedSPI = spi;
	CE = nrf_ce;
}

void NRF::WriteByte(char byt)
{
}
char NRF::ReadByte()
{
	return 0;
}
int NRF::Seek(long long offset)
{
	return 0;
}
void NRF::DefaultInit(char *addr, int mode)
{
	Init(addr, NRFDefaultParameters.Channel, NRFDefaultParameters.BaudRate, NRFDefaultParameters.CRCEncodingScheme, NRFDefaultParameters.PacketSize, mode);
}
void NRF::DefaultInit(char a1, char a2, char a3, char a4, char a5, int mode)
{
	char addr[] = { a1, a2, a3, a4, a5 };
	Init(addr, NRFDefaultParameters.Channel, NRFDefaultParameters.BaudRate, NRFDefaultParameters.CRCEncodingScheme, NRFDefaultParameters.PacketSize, mode);
}
void NRF::Init(char *addr, int channel, int baudRateType, int crcEncodingScheme, int packetSize, int mode)
{
	Reset();
	PowerUp();
	SetRXAddr(addr);
	SetTXAddr(addr);
	SetBaudRate(baudRateType);
	SetCRCEncodingScheme(crcEncodingScheme);
	SetRXPacketSize(packetSize);
	SetChannel(40);
	SetMode(mode);
	SetPipeStatus(0x01);
	SetRetranTimes(15);
	SetRetranInterval(500);
	ClearTXStatus();
	ClearRXStatus();
	ClearMaxTXStatus();
	FlushTX();
	FlushRX();
}
void NRF::Init(char a1, char a2, char a3, char a4, char a5, int channel, int baudRateType, int crcEncodingScheme, int packetSize, int mode)
{
	char addr[] = { a1, a2, a3, a4, a5 };
	Init(addr, channel, baudRateType, crcEncodingScheme, packetSize, mode);
}
void NRF::FlushTX()
{
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.FlushTX);
	OccuppiedSPI->ChipRelease();
}
void NRF::FlushRX()
{
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.FlushRX);
	OccuppiedSPI->ChipRelease();
}
char NRF::GetRegValue(char addr)
{
	char value = 0;
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.ReadRegester + addr);
	OccuppiedSPI->WriteByte(0xff);
	value = OccuppiedSPI->ReadByte();
	OccuppiedSPI->ChipRelease();
	return value;
}
void NRF::GetRegValue(char addr, int length, char *value)
{
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.ReadRegester + addr);
	for(int i = 0; i < length; i++)
	{
		OccuppiedSPI->WriteByte(0xff);
		value[i] = OccuppiedSPI->ReadByte();
	}
	OccuppiedSPI->ChipRelease();
}
void NRF::SetRegValue(char addr, char value)
{
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.WriteRegester + addr);
	OccuppiedSPI->WriteByte(value);
	OccuppiedSPI->ChipRelease();
}
void NRF::SetRegValue(char addr, char *value, int length)
{
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.WriteRegester + addr);
	for(int i = 0; i < length; i++)
	{
		OccuppiedSPI->WriteByte(value[i]);
	}
	OccuppiedSPI->ChipRelease();
}
void NRF::Reset()
{
	Deactivate();
	OccuppiedSPI->ChipRelease();
}
void NRF::SetRetranTimes(int times)
{
	if(times < 0 || times > 15)
	{
		return ;
	}
	char status = GetRegValue(NRFRegesters.SETUP_RETR);
	status &= ~ 0x0f;
	status |= times;
	SetRegValue(NRFRegesters.SETUP_RETR, status);
}
int  NRF::GetRetranTimes()
{
	return GetRegValue(NRFRegesters.SETUP_RETR) & 0x0f;
}
void NRF::SetRetranInterval(int intervalUs)
{
	if(intervalUs < 0 || intervalUs > 10000)
	{
		return ;
	}
	char status = GetRegValue(NRFRegesters.SETUP_RETR);
	status &= ~ 0xf0;
	if(intervalUs > 250)
	{
		for(int i = 1; i < 16; i++)
		{
			if(250 * i < intervalUs && 250 * i + 250 >= intervalUs)
			{
				if(intervalUs % 250 > 125)
				{
					status |= i << 4;
				}
				else
				{
					status |= (i - 1) << 4;
				}
				break;
			}
		}
		if(!(status & 0xf0))
		{
			status |= 0xf0;
		}
	}
	SetRegValue(NRFRegesters.SETUP_RETR, status);
}
int  NRF::GetRetranInterval()
{
	return (GetRegValue(NRFRegesters.SETUP_RETR) >> 4) * 250 + 250;
}
void NRF::SetTXAddr(char a1, char a2, char a3, char a4, char a5)
{
	char addr[5];
	addr[0] = a1; addr[1] = a2; addr[2] = a3; addr[3] = a4; addr[4] = a5;
	SetTXAddr(addr);
}
void NRF::SetTXAddr(char *addr)
{
	SetRegValue(NRFRegesters.TX_ADDR, addr, 5);
}
void NRF::GetRXAddr(char *addr)
{
	GetRegValue(NRFRegesters.RX_ADDR_P0, 5, addr);
}
void NRF::GetTXAddr(char *addr)
{
	GetRegValue(NRFRegesters.TX_ADDR, 5, addr);
}
void NRF::SetRXAddr(char a1, char a2, char a3, char a4, char a5)
{
	char addr[5];
	addr[0] = a1; addr[1] = a2; addr[2] = a3; addr[3] = a4; addr[4] = a5;
	SetRXAddr(addr);
}
void NRF::SetRXAddr(char *addr)
{
	SetRegValue(NRFRegesters.RX_ADDR_P0, addr, 5);
}
void NRF::SetAddr(char a1, char a2, char a3, char a4, char a5)
{
	char addr[5];
	addr[0] = a1; addr[1] = a2; addr[2] = a3; addr[3] = a4; addr[4] = a5;
	SetTXAddr(addr);
	SetRXAddr(addr);
}
void NRF::SetAddr(char *addr)
{
	SetTXAddr(addr);
	SetRXAddr(addr);
}
void NRF::GetAddr(char *addr)
{
	GetTXAddr(addr);
}
void NRF::SetMode(int mode)
{
	char status = GetRegValue(NRFRegesters.CONFIG);
	status &= ~NRFModes.Receive;
	status |= mode;
	SetRegValue(NRFRegesters.CONFIG, status);
}
int NRF::GetMode()
{
	return GetRegValue(NRFRegesters.CONFIG) & NRFModes.Receive;
}
void NRF::SetRXPacketSize(int size)
{
	SetRegValue(NRFRegesters.RX_PW_P0, (PacketSize = size));
}
int NRF::GetRXPacketSize()
{
	return (PacketSize = GetRegValue(NRFRegesters.RX_PW_P0));
}
bool NRF::GetTXStatus()
{
	return !!(GetRegValue(NRFRegesters.STATUS) & (1 << 5));
}
bool NRF::GetRXStatus()
{
	return !!(GetRegValue(NRFRegesters.STATUS) & (1 << 6));
}
bool NRF::GetMaxTXStatus()
{
	return !!(GetRegValue(NRFRegesters.STATUS) & (1 << 4));
}
void NRF::ClearTXStatus()
{
	SetRegValue(NRFRegesters.STATUS, 1 << 5);
}
void NRF::ClearRXStatus()
{
	SetRegValue(NRFRegesters.STATUS, 1 << 6);
}
void NRF::ClearMaxTXStatus()
{
	SetRegValue(NRFRegesters.STATUS, 1 << 4);
}
void NRF::SetBaudRate(int baudRateType)
{
	SetRegValue(NRFRegesters.RF_SETUP, baudRateType | 0x07);
}
int  NRF::GetBaudRate()
{
	return GetRegValue(NRFRegesters.RF_SETUP) & 0x28;
}
void NRF::SetCRCEncodingScheme(int crcEncoding)
{
	char status = GetRegValue(NRFRegesters.CONFIG);
	status &= ~NRFCRCEncodingSchemes.CRC2Bytes;
	status |= crcEncoding;
	SetRegValue(NRFRegesters.CONFIG, status);
}
int  NRF::GetCRCEncodingScheme()
{
	return GetRegValue(NRFRegesters.CONFIG) & NRFCRCEncodingSchemes.CRC2Bytes;
}
void NRF::PowerDown()
{
	char status = GetRegValue(NRFRegesters.CONFIG);
	status &= ~0x02;
	SetRegValue(NRFRegesters.CONFIG, status);
}
void NRF::PowerUp()
{
	char status = GetRegValue(NRFRegesters.CONFIG);
	status |= 0x02;
	SetRegValue(NRFRegesters.CONFIG, status);
}
void NRF::Activate()
{
	*CE = 1;
}
void NRF::Deactivate()
{
	*CE = 0;
}

void NRF::ReadBuffer(char *buffer)
{
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.ReadBuffer);
	for(int i = 0; i < PacketSize; i++)
	{
		OccuppiedSPI->WriteByte(0xff);
		buffer[i] = OccuppiedSPI->ReadByte();
	}
	OccuppiedSPI->ChipRelease();
	FlushRX();
}
void NRF::WriteBuffer(char *buffer)
{
	FlushTX();
	ClearTXStatus();
	ClearMaxTXStatus();
	OccuppiedSPI->ChipSetect();
	OccuppiedSPI->WriteByte(NRFCommands.WriteBuffer);
	for(int i = 0; i < PacketSize; i++)
	{
		OccuppiedSPI->WriteByte(buffer[i]);
	}
	OccuppiedSPI->ChipRelease();
}
void NRF::SetChannel(int channel)
{
	SetRegValue(NRFRegesters.RF_CH, (Channel = channel));
}
int NRF::GetChannel()
{
	return (Channel = GetRegValue(NRFRegesters.RF_CH));
}
void NRF::SetPipeStatus(int pipeIndex, bool newState)
{
	if(pipeIndex > 5 || pipeIndex < 0)
	{
		return ;
	}
	PipeStatus = GetPipeStatus();
	if(newState)
	{
		PipeStatus |= (1 << pipeIndex);
	}
	else
	{
		PipeStatus &= ~(1 << pipeIndex);
	}
	SetRegValue(NRFRegesters.EN_AA, PipeStatus);
	SetRegValue(NRFRegesters.EN_RXADDR, PipeStatus);
}
bool NRF::GetPipeStatus(int pipeIndex)
{
	return !!(((PipeStatus = GetRegValue(NRFRegesters.EN_AA)) >> pipeIndex) & 1);
}
char NRF::GetPipeStatus()
{
	return PipeStatus = GetRegValue(NRFRegesters.EN_AA);
}
void NRF::SetPipeStatus(char status)
{
	SetRegValue(NRFRegesters.EN_AA, status);
	SetRegValue(NRFRegesters.EN_RXADDR, status);
}

