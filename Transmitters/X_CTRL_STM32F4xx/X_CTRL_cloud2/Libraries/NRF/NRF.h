#ifndef __NRF_H__
#define __NRF_H__

#include "arduino.h"
#include "SoftwareSpi.h"

typedef uint32_t NRF_PORT_TYPEDEF;

#define NRF_ResetPin(n)			digitalWrite_LOW(n)
#define NRF_SetPin(n)				digitalWrite_HIGH(n)
#define NRF_GetPin(n)				digitalRead_Fast(n)

/* 定义各种寄存器、命令等等 */

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

extern NRFBaudRates_TypeDef NRFBaudRates;
extern NRFModes_TypeDef NRFModes;
extern NRFCRCEncodingSchemes_TypeDef NRFCRCEncodingSchemes;
extern NRFCommands_TypeDef NRFCommands;
extern NRFRegesters_TypeDef NRFRegesters;

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
		BaudRate = NRFBaudRates._2Mbps;
		CRCEncodingScheme = NRFCRCEncodingSchemes.CRC2Bytes;
		PacketSize = 32;
		Mode = NRFModes.Receive;
	}
};

extern NRFParameters_TypeDef NRFDefaultParameters;

/* ------------------ NRF ʹ��˵�� ------------------
	 NRF ģ��ĹܽŶ���һ��Ϊ��

	 [ NRF �ܽ�ģʽ ]
    --------------------------------------------------------------------------------------
	 |                                                                                      |
	 | �� ��һ�ܽ�                                                                          |
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

		
	 �����Ҫ 2 �� NRF ģ��������ݴ��䣬һ
	 ������£������뱣֤������Щ����������
	 NRF ģ���ϵ�ֵ����һ�£�

	 �����͵�ַ				�� TX_ADDR �Ĵ���
	 �����յ�ַ				�� RX_ADDR_P0 �Ĵ���
	 ����ַ����				�� SETUP_AW �Ĵ��� [1:0]
	 ��Ƶ��						�� RF_CH �Ĵ���
	 ��������					�� RF_SETUP �Ĵ��� [5:5] �� [3:3]
	 ��CRC ���뷽ʽ		�� SETUP �Ĵ��� [3:3]
	 �����ݰ���С*		�� RX_PW_P0 �Ĵ���
	 
	 ���ң����� NRF ģ����Ҫ��Щ���ã�
	 ����������״̬��PowerUp��
	 ��CE �Ŵ��ڸߵ�ƽ״̬
	 
	 *���ݰ���С�����ݰ���С��Ӱ�� CRC У��ֵ���������ݰ�
								��С��ͬʱ��Ȼ�޷������շ�����
								
	 ʹ�� DefaultInit ��������ʹ��Щ������������Ϊ
	 һ�µġ����ʵ�ֵ����ЩֵΪ��
	 
	 �����͵�ַ				(���û��ṩ)
	 �����յ�ַ				(���û��ṩ)
	 ����ַ����				5 ���ֽ�
	 ��Ƶ��						ʮ����ֵ 40 
	 ��������					1 Mbps
	 ��CRC ���뷽ʽ		2 �ֽ�
	 �����ݰ���С			32 �ֽ�
	 ��ģʽ						����ģʽ
	 ��CE ״̬				ʹ��
	 ����Դ״̬				����
	 ���շ�ͨ��				ͨ��0
	 ��Щֵ������ NRFDefaultParameters ö�ٶ����в�ѯ
	 ��ʹ�� Init ������ͨ���Զ�����Щ�����ķ�ʽʹ NRF 
	 ���õ�����״̬
	 
	 
*/
class NRF
{
public :
	SoftwareSPI *OccuppiedSPI;
  NRF_PORT_TYPEDEF CE;
	int PacketSize;
	int Channel;
	char TXAddr[5];
	char RXAddr[5];
	char PipeStatus;
	
	NRF();
	NRF(SoftwareSPI *spi);
	NRF(SoftwareSPI *spi, NRF_PORT_TYPEDEF nrf_ce);
	void Reset();
	bool DefaultInit(char *addr, int mode);
	bool DefaultInit(char a1, char a2, char a3, char a4, char a5, int mode);
	bool Init(char *addr, int channel, int baudRateType, int crcEncodingScheme, int packetSize, int mode);
	bool Init(char a1, char a2, char a3, char a4, char a5, int channel, int baudRateType, int crcEncodingScheme, int packetSize, int mode);
	void FlushTX();															/* 清空 TX 缓冲区 */
	void FlushRX();															/* 清空 RX 缓冲区 */
	char GetRegValue(char addr);											/* 读取寄存器值 */
	void GetRegValue(char addr, int length, char *value);					/* 读取寄存器值 */
	void SetRegValue(char addr, char value);								/* 设置寄存器值 */
	void SetRegValue(char addr, char *value, int length);					/* 设置寄存器值 */
	void SetTXAddr(char a1, char a2, char a3, char a4, char a5);			/* 设置发送地址 */
	void SetTXAddr(char *addr);												/* 设置发送地址 */
	void GetTXAddr(char *addr);												/* 获取寄存器中的发送地址 */
	void SetRXAddr(char a1, char a2, char a3, char a4, char a5);			/* 获取寄存器中的发送地址 */
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
};





#endif


