#include "shellclient.h"

/* create packet */
static void ShellClient_GetPacket(char *buffer, int cmd, int id, int psize, char *params)
{
	char checksum = 0;
	checksum += buffer[0] = cmd;
	checksum += buffer[1] = id;
	checksum += buffer[2] = psize;
	for(int i = 0; i < SHELL_MAXPARAMSSIZE; i++)
	{
		if(i < psize)
		{
			checksum += buffer[i + 3] = params[i];
		}
		else
		{
			checksum += buffer[i + 3] = 0;
		}
	}
	buffer[SHELL_PACKETSIZE - 1] = checksum;
}

ShellClient::ShellClient(const char *name, bool(*sendPacket)(char *buff))
{
	Handled = true;
	FuncCount = 0;
	LastRetStatus = false;
	LastParamsSize = 0;
	LastFuncCountStatus = false;
	LastFuncNameStatus = false;
	strcpy(MyName, name);
	SendPacket = sendPacket;
	/* reset params */
	for(int i = 0; i < SHELL_MAXPARAMSSIZE; i++)
	{
		LastParams[i] = 0;
	}
	/* reset func list */
	for(int i = 0; i < SHELL_MAXFUNC; i++)
	{
		Functions[i] = 0;
	}
	/* reset tx&rx buffer */
	for(int i = 0; i < SHELL_PACKETSIZE; i++)
	{
		TXBuffer[i] = RXBuffer[i] = 0;
	}
}

int ShellClient::RegisterFunc(ExposedFunc_TypeDef proc, const char *name)
{
	if(FuncCount < SHELL_MAXFUNC)
	{
		strcpy(RegisterFuncNames[FuncCount], name);
		Functions[FuncCount] = proc;
		return FuncCount ++;
	}
	else
	{
		return -1;
	}
}
bool ShellClient::BeginCall(const char *fname)
{
	return BeginCall(fname, 0, 0);
}
bool ShellClient::BeginCall(const char *fname, uint8_t params)
{
	return BeginCall(fname, sizeof(uint8_t), &params);
}
bool ShellClient::BeginCall(const char *fname, int8_t params)
{
	return BeginCall(fname, sizeof(int8_t), &params);
}
bool ShellClient::BeginCall(const char *fname, uint16_t params)
{
	return BeginCall(fname, sizeof(uint16_t), &params);
}
bool ShellClient::BeginCall(const char *fname, int16_t params)
{
	return BeginCall(fname, sizeof(int16_t), &params);
}
bool ShellClient::BeginCall(const char *fname, uint32_t params)
{
	return BeginCall(fname, sizeof(uint32_t), &params);
}
bool ShellClient::BeginCall(const char *fname, int32_t params)
{
	return BeginCall(fname, sizeof(int32_t), &params);
}
bool ShellClient::BeginCall(const char *fname, uint64_t params)
{
	return BeginCall(fname, sizeof(uint64_t), &params);
}
bool ShellClient::BeginCall(const char *fname, int64_t params)
{
	return BeginCall(fname, sizeof(int64_t), &params);
}
bool ShellClient::BeginCall(const char *fname, float params)
{
	return BeginCall(fname, sizeof(float), &params);
}
bool ShellClient::BeginCall(const char *fname, double params)
{
	return BeginCall(fname, sizeof(double), &params);
}
bool ShellClient::BeginCall(const char *fname, int pSize, void *params)
{
	for(int i = 0; i < TargetFuncCount; i++)
	{
		if(!strcmp(TargetFuncNames[i], fname))
		{
			return BeginCall(i, pSize, (char *)params);
		}
	}
	return false;
}
bool ShellClient::BeginCall(int id, int pSize, char *params)
{
	ClearRetStatus();
	ShellClient_GetPacket(TXBuffer, SHELL_CMD_CallFunc, id, pSize, params);
	return SendPacket(TXBuffer);
}
bool ShellClient::BeginGetTargetName()
{
	ShellClient_GetPacket(TXBuffer, SHELL_CMD_GetName, 0, 0, 0);
	return SendPacket(TXBuffer);
}
bool ShellClient::BeginGetFuncName(int id)
{
	ShellClient_GetPacket(TXBuffer, SHELL_CMD_GetFuncName, id, 0, 0);
	return SendPacket(TXBuffer);
}
bool ShellClient::BeginGetFuncCount()
{
	ShellClient_GetPacket(TXBuffer, SHELL_CMD_GetFuncCnt, 0, 0, 0);
	return SendPacket(TXBuffer);
}
int ShellClient::GetRetValue(void *params)
{
	if(LastRetStatus)
	{
		LastRetStatus = false;
		for(int i = 0; i < LastParamsSize; i++)
		{
			((char *)params)[i] = LastParams[i];
		}
		return LastParamsSize;
	}
	else
	{
		return -1;
	}
}
uint8_t ShellClient::GetUINT8()
{
	uint8_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
int8_t ShellClient::GetINT8()
{
	int8_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
uint16_t ShellClient::GetUINT16()
{
	uint16_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
int16_t ShellClient::GetINT16()
{
	int16_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
uint32_t ShellClient::GetUINT32()
{
	uint32_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
int32_t ShellClient::GetINT32()
{
	int32_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
uint64_t ShellClient::GetUINT64()
{
	uint64_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
int64_t ShellClient::GetINT64()
{
	int64_t dat = 0;
	GetRetValue(&dat);
	return dat;
}
float ShellClient::GetFloat()
{
	float dat = 0;
	GetRetValue(&dat);
	return dat;
}
double ShellClient::GetDouble()
{
	double dat = 0;
	GetRetValue(&dat);
	return dat;
}

int ShellClient::GetTargetFuncCount()
{
	if(LastFuncCountStatus)
	{
		return TargetFuncCount;
	}
	else
	{
		return -1;
	}
}
int ShellClient::GetTargetName(char *str)
{
	if(LastTargetNameStatus)
	{
		strcpy(str, TargetName);
		return 0;
	}
	else
	{
		return -1;
	}
}
void ShellClient::GetTargetFuncName(int id, char *str)
{
	strcpy(str, TargetFuncNames[id]);
}
bool ShellClient::GetTargetNameStatus()
{
	return LastTargetNameStatus;
}
bool ShellClient::GetRetStatus()
{
	return LastRetStatus;
}
bool ShellClient::GetFuncCountStatus()
{
	return LastFuncCountStatus;
}
bool ShellClient::GetFuncNameStatus()
{
	return LastFuncNameStatus;
}
void ShellClient::ClearRetStatus()
{
	LastRetStatus = false;
}
void ShellClient::ClearFuncCountStatus()
{
	LastFuncCountStatus = false;
}
void ShellClient::ClearFuncNameStatus()
{
	LastFuncNameStatus = false;
}
void ShellClient::ClearTargetNameStatus()
{
	LastTargetNameStatus = false;
}
void ShellClient::PacketReceived(char *buff)
{
	if(Handled)
	{
		for(int i = 0; i < SHELL_PACKETSIZE; i++)
		{
			RXBuffer[i] = buff[i];
		}
		uint8_t cmd = RXBuffer[0];
		uint8_t checksum = RXBuffer[SHELL_PACKETSIZE - 1];
		uint8_t n_checksum = 0;
		for(int i = 0; i < SHELL_PACKETSIZE - 1; i++)
		{
			n_checksum += RXBuffer[i];
		}
		if(n_checksum == checksum)
		{
			Handled = false;
			Executed = cmd != SHELL_CMD_CallFunc;
		}
	}
}
bool ShellClient::Handle()
{
	if(!Handled)
	{
		bool success = true;
		uint8_t cmd = RXBuffer[0];
		uint8_t id = RXBuffer[1];
		uint8_t psize = RXBuffer[2];
		char *params = RXBuffer + 3;
		
		switch(cmd)
		{
			case SHELL_CMD_GetName:
			{
				ShellClient_GetPacket(TXBuffer, 
															SHELL_CMD_AckName,
															0, strlen(MyName), MyName);
				success &= SendPacket(TXBuffer);
			}
			break;
			
			case SHELL_CMD_AckName:
			{
				LastTargetNameStatus = true;
				strcpy(TargetName, params);
			}
			break;
			
			case SHELL_CMD_GetFuncCnt:
			{
				ShellClient_GetPacket(TXBuffer, 
															SHELL_CMD_AckFuncCnt,
															0, 1, (char *)&FuncCount);
				success &= SendPacket(TXBuffer);
			}
			break;
			
			case SHELL_CMD_AckFuncCnt:
			{
				LastFuncCountStatus = true;
				TargetFuncCount = *params;
			}
			break;
			
			case SHELL_CMD_GetFuncName:
			{
				ShellClient_GetPacket(TXBuffer, 
															SHELL_CMD_AckFuncName,
															id, strlen(RegisterFuncNames[id]), RegisterFuncNames[id]);
				success &= SendPacket(TXBuffer);
			}
			break;
			
			case SHELL_CMD_AckFuncName:
			{
				LastFuncNameStatus = true;
				strcpy(TargetFuncNames[id], params);
			}
			break;
			
			case SHELL_CMD_CallFunc:
			{
				if(!Executed)
				{
					Executed = true;
					RetPSize = Functions[id](psize, params);
					for(int i = 0; i < SHELL_MAXPARAMSSIZE; i++)
					{
						Params[i] = params[i];
					}
				}
				ShellClient_GetPacket(TXBuffer, 
															SHELL_CMD_RetFunc,
															id, RetPSize, Params);
				success &= SendPacket(TXBuffer);
			}
			break;
			
			case SHELL_CMD_RetFunc:
			{
				LastRetStatus = true;
				LastParamsSize = psize;
				for(int i = 0; i < psize; i++)
				{
					LastParams[i] = params[i];
				}
			}
			break;
		}
		
		if(success)
		{
			Handled = true;
		}
		return success;
	}
	else
	{
		return false;
	}
}
