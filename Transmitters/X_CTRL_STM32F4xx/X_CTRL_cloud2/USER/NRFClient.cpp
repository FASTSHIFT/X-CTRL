#include "nrfclient.h"
#include "filegroup.h"

#define NRF_ADDR			10, 93, 70, 72, 31
#define NRF_CLIENTCH	67

SoftwareSPI NRF_SPI(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CSN_Pin);
NRF nrf(&NRF_SPI, NRF_CE_Pin);

char NRF_RXBuffer[32];
char NRF_TXBuffer[32];

bool Client_SendPacket(char *packet);
ShellClient client("controller", Client_SendPacket);

uint32_t Client_Call(const char *fname, int pSize, void *params, void *ret)
{
	static uint32_t lastMS = 0;
	while(millis() - lastMS < 20);
	volatile uint32_t beginMS = millis();
	bool callStatus = false;
	bool retStatus = false;
	while(!client.Handled)
	{
		client.Handle();
	}
	do
	{
		client.ClearRetStatus();
		volatile uint32_t callMS = millis();
		client.BeginCall(fname, pSize, params);
		volatile uint32_t retMS = millis();
		while(!client.GetRetStatus() && millis() - retMS < Client_Timeout) { Client_Handle(); }
		if(!!(retStatus = client.GetRetStatus()))
		{
			client.GetRetValue(ret);
			break;
		}
		else if(millis() - beginMS > Client_CallTimeout)
		{
			break;
		}
	}
	while(true);
	lastMS = millis();
	return retStatus ? (millis() - beginMS) : 0;
}

void Client_Handle()
{
	static uint64_t lastReceived = millis();
	
	if(nrf.GetRXStatus())
	{
		nrf.Deactivate();
		nrf.ClearRXStatus();
		nrf.ReadBuffer(NRF_RXBuffer);
		client.PacketReceived(NRF_RXBuffer);
		nrf.Activate();
	}
	client.Handle();
}

bool Client_SendPacket(char *packet)
{
	nrf.Deactivate();
	nrf.SetMode(NRFModes.Transmission);
	nrf.WriteBuffer(packet);
	nrf.ClearTXStatus();
	nrf.ClearMaxTXStatus();
	nrf.Activate();
	volatile uint32_t now = millis();
	while(millis() - now < 50)
	{
		if(nrf.GetTXStatus())
		{
			nrf.ClearTXStatus();
			nrf.Deactivate();
			nrf.SetMode(NRFModes.Receive);
			nrf.Activate();
			return true;
		}
		else if(nrf.GetMaxTXStatus())
		{
			nrf.ClearMaxTXStatus();
			nrf.Deactivate();
			nrf.SetMode(NRFModes.Receive);
			nrf.Activate();
			return false;
		}
	}
	nrf.Deactivate();
	nrf.SetMode(NRFModes.Receive);
	nrf.Activate();
	return false;
}

void Init_Client()
{
	pinMode(NRF_MOSI_Pin, OUTPUT);
	pinMode(NRF_MISO_Pin, INPUT);
	pinMode(NRF_SCK_Pin, OUTPUT);
	pinMode(NRF_CSN_Pin, OUTPUT);
	pinMode(NRF_CE_Pin, OUTPUT);
	
	delay(200);
	
	nrf.DefaultInit(NRF_ADDR, NRFModes.Receive);
	nrf.SetChannel(NRF_CLIENTCH);
}
