#ifndef __NRF_H__
#define __NRF_H__

#include "object.h"
#include "stream.h"
#include "softwareSpi.h"

class NRFCommands_TypeDef
{
	public :
		char ReadRegester;
		char WriteRegester;
		char ReadBuffer;
		char WriteBuffer;
		char FlushTX;
		char FlushRX;
	NRFCommands_TypeDef() : ReadRegester(0), WriteRegester(1 << 5), ReadBuffer(0x61), WriteBuffer(0xA0), FlushTX(0xE1), FlushRX(0xE2) { }
};

class NRFRegesters_TypeDef
{
	public :
		char CONFIG;
		char EN_AA;
		char EN_RXADDR;
		char SETUP_AW;
		char SETUP_RETR;
		char RF_CH;
		char RF_SETUP;
		char STATUS;
		char OBSERVE_TX;
		char RPD;
		char RX_ADDR_P0;
		char RX_ADDR_P1;
		char RX_ADDR_P2;
		char RX_ADDR_P3;
		char RX_ADDR_P4;
		char RX_ADDR_P5;
		char TX_ADDR;
		char RX_PW_P0;
		char RX_PW_P1;
		char RX_PW_P2;
		char RX_PW_P3;
		char RX_PW_P4;
		char RX_PW_P5;
		char FIFO_STATUS;
		char ACK_PLD;
		char TX_PLD;
		char RX_PLD;
		char DYNPD;
		char FEATURE;
		NRFRegesters_TypeDef() : 
		CONFIG(0),
		EN_AA(1),
		EN_RXADDR(2),
		SETUP_AW(3),
		SETUP_RETR(4),
		RF_CH(5),
		RF_SETUP(6),
		STATUS(7),
		OBSERVE_TX(8),
		RPD(9),
		RX_ADDR_P0(10),
		RX_ADDR_P1(11),
		RX_ADDR_P2(12),
		RX_ADDR_P3(13),
		RX_ADDR_P4(14),
		RX_ADDR_P5(15),
		TX_ADDR(16),
		RX_PW_P0(17),
		RX_PW_P1(18),
		RX_PW_P2(19),
		RX_PW_P3(20),
		RX_PW_P4(21),
		RX_PW_P5(22),
		FIFO_STATUS(23),
		ACK_PLD(0),
		TX_PLD(0),
		RX_PLD(0),
		DYNPD(28),
		FEATURE(29)
		{
		}
};
class NRFModes_TypeDef
{
	public :
	int Transmission;
	int Receive;
	NRFModes_TypeDef() : Transmission(0), Receive(1) { }
};
class NRFBaudRates_TypeDef
{
	public :
	int _250Kbps;
	int _1Mbps;
	int _2Mbps;
	NRFBaudRates_TypeDef() : _250Kbps(0x20), _1Mbps(0x00), _2Mbps(0x08) { }
};
class NRFCRCEncodingSchemes_TypeDef
{
	public :
	int CRC1Byte;
	int CRC2Bytes;
	NRFCRCEncodingSchemes_TypeDef() : CRC1Byte(0), CRC2Bytes(0x04) { }
};

extern NRFCommands_TypeDef NRFCommands;
extern NRFRegesters_TypeDef NRFRegesters;
extern NRFModes_TypeDef NRFModes;
extern NRFBaudRates_TypeDef NRFBaudRates;
extern NRFCRCEncodingSchemes_TypeDef NRFCRCEncodingSchemes;

class NRFParameters_TypeDef
{
	public :
	int Channel;
	int BaudRate;
	int CRCEncodingScheme;
	int PacketSize;
	int Mode;
	NRFParameters_TypeDef()
	{
		Channel = 40;
		BaudRate = NRFBaudRates._250Kbps;
		CRCEncodingScheme = NRFCRCEncodingSchemes.CRC2Bytes;
		PacketSize = 32;
		Mode = NRFModes.Receive;
	}
};
extern NRFParameters_TypeDef NRFDefaultParameters;


/* ------------------ NRF 使用说明 ------------------
	 NRF 模块的管脚定义一般为：

	 [ NRF 管脚模式 ]
    --------------------------------------------------------------------------------------
	 |                                                                                      |
	 | ↓ 第一管脚                                                                          |
	  -----------------------------                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
   |   [1.GND]   |      VCC     |                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
	  -----------------------------                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
   |     CE      |      CSN     |                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
	  -----------------------------                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
   |     SCK     |     MOSI     |                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
	  -----------------------------                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
   |     MISO    |     IRQ      |                                                         |
   |             |              |                                                         |
   |             |              |                                                         |
	  --------------------------------------------------------------------------------------

		
	 如果需要 2 个 NRF 模块进行数据传输，一
	 般情况下，您必须保证以下这些参数在两个
	 NRF 模块上的值保持一致：

	 ·发送地址				在 TX_ADDR 寄存器
	 ·接收地址				在 RX_ADDR_P0 寄存器
	 ·地址长度				在 SETUP_AW 寄存器 [1:0]
	 ·频道						在 RF_CH 寄存器
	 ·波特率					在 RF_SETUP 寄存器 [5:5] 和 [3:3]
	 ·CRC 编码方式		在 SETUP 寄存器 [3:3]
	 ·数据包大小*		在 RX_PW_P0 寄存器
	 
	 并且，两个 NRF 模块需要这些配置：
	 ·处于启用状态（PowerUp）
	 ·CE 脚处于高电平状态
	 
	 *数据包大小：数据包大小会影响 CRC 校验值，于是数据包
								大小不同时仍然无法正常收发数据
								
	 使用 DefaultInit 函数，以使这些参数都被配置为
	 一致的、合适的值，这些值为：
	 
	 ·发送地址				(由用户提供)
	 ·接收地址				(由用户提供)
	 ·地址长度				5 个字节
	 ·频道						十进制值 100 
	 ·波特率					250 Kbps
	 ·CRC 编码方式		2 字节
	 ·数据包大小			32 字节
	 ·模式						接收模式
	 ·CE 状态				使能
	 ·电源状态				开启
	 ·收发通道				通道0
	 这些值可以在 NRFDefaultParameters 枚举对象中查询
	 或使用 Init 函数，通过自定义这些参数的方式使 NRF 
	 配置到就绪状态
	 
	 
*/

class NRF : public Stream
{
public :
	SoftwareSPI *OccuppiedSPI;
	int *CE;
	int PacketSize;
	int Channel;
	char TXAddr[5];
	char RXAddr[5];
	char PipeStatus;
	
	NRF();
	NRF(SoftwareSPI *spi);
	NRF(SoftwareSPI *spi, int *nrf_ce);
	void Reset();
	void DefaultInit(char *addr, int mode);
	void DefaultInit(char a1, char a2, char a3, char a4, char a5, int mode);
	void Init(char *addr, int channel, int baudRateType, int crcEncodingScheme, int packetSize, int mode);
	void Init(char a1, char a2, char a3, char a4, char a5, int channel, int baudRateType, int crcEncodingScheme, int packetSize, int mode);
	void FlushTX();
	void FlushRX();
	char GetRegValue(char addr);
	void GetRegValue(char addr, int length, char *value);
	void SetRegValue(char addr, char value);
	void SetRegValue(char addr, char *value, int length);
	void SetTXAddr(char a1, char a2, char a3, char a4, char a5);
	void SetTXAddr(char *addr);
	void GetTXAddr(char *addr);
	void SetRXAddr(char a1, char a2, char a3, char a4, char a5);
	void SetRXAddr(char *addr);
	void GetRXAddr(char *addr);
	void SetAddr(char *addr);
	void SetAddr(char a1, char a2, char a3, char a4, char a5);
	void GetAddr(char *addr);
	void SetMode(int mode);
	int  GetMode();
	void SetRetranTimes(int times);
	int  GetRetranTimes();
	void SetRetranInterval(int intervalUs);
	int  GetRetranInterval();
	void SetRXPacketSize(int size);
	int  GetRXPacketSize();
	bool GetTXStatus();
	bool GetRXStatus();
	bool GetMaxTXStatus();
	void ClearTXStatus();
	void ClearRXStatus();
	void ClearMaxTXStatus();
	void ReadBuffer(char *buffer);
	void WriteBuffer(char *buffer);
	void SetChannel(int channel);
	int  GetChannel();
	void SetPipeStatus(int pipeIndex, bool newState);
	void SetPipeStatus(char status);
	bool GetPipeStatus(int pipeIndex);
	char GetPipeStatus();
	void SetBaudRate(int baudRateType);
	int  GetBaudRate();
	void SetCRCEncodingScheme(int crcEncoding);
	int  GetCRCEncodingScheme();
	void PowerDown();
	void PowerUp();
	void Activate();
	void Deactivate();

	virtual void WriteByte(char byt);
	virtual char ReadByte();
	virtual int Seek(long long offset);
};





#endif


