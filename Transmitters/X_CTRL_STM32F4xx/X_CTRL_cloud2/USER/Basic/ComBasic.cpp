#include "FileGroup.h"
#include "ComPrivate.h"

/*实例化NRF对象*/
NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

ShellClient client("JW_Controller", NRF_SendPacket);
uint8_t Client_SlaverCount = 0;

/*NRF发送数据缓冲区*/
uint8_t NRF_TxBuff[32];

/*NRF接收数据缓冲区*/
uint8_t NRF_RxBuff[32];

/*发送数据使能*/
bool State_RF = OFF;

/*回传使能*/
bool State_PassBack = OFF;

/*握手使能*/
bool State_Handshake = ON;

void NRF_Handle()
{
	uint8_t status =  nrf.Recv(NRF_RxBuff);
	if(status & nrf.RX_DR)
	{
		client.PacketReceived((char *)NRF_RxBuff);
	}
	client.Handle();
}

bool NRF_SendPacket(char *buffer)
{
	volatile uint32_t nowMS = millis();
	nrf.TX_Mode((uint8_t *)buffer);
	
	while(millis() - nowMS < 100)
	{
		delay(20);
		uint8_t status = nrf.Tran((uint8_t *)buffer);
		if(status & nrf.TX_DS)
		{
			nrf.RX_Mode();
			return true;
		}
		else if(status & nrf.MAX_RT)
		{
			nrf.RX_Mode();
			return false;
		}
	}
	nrf.RX_Mode();
	return false;
}


void Init_Client()
{
	/* 主机准备握手 */
//	HandshakeRun(HandshakeState_Prepare);
//	/* 超时设置5000ms */
//	uint32_t time = millis();
//	
//	uint8_t slaverCnt = 0;
//	/* 搜索从机 */
//	while(millis() - time < 3000)
//	{
//		/*获取从机列表数量*/
//		slaverCnt = HandshakeRun(HandshakeState_Search);
//	}
//	
//	Client_SlaverCount = slaverCnt;
}

/**
  * @brief  NRF初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Init_NRF()
{
    /*总初始化*/
    nrf.init(nrf.TX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
	nrf.SetAddress(NRF_ADDR);
	nrf.SetSpeed(nrf.SPEED_2Mbps);
	nrf.SetFreqency(NRF_CLIENTCH);

    /*返回连接情况*/
    return nrf.IsDetect();
}

/**
  * @brief  数据发送任务
  * @param  无
  * @retval 无
  */
void Task_TransferData()
{
    if(State_RF == OFF) return;
}
