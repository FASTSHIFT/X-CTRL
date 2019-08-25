#ifndef __SHELLCLIENT_H__
#define __SHELLCLIENT_H__

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define SHELL_MAXFUNC					64
#define SHELL_PACKETSIZE			32
#define SHELL_MAXPARAMSSIZE		28

/* Pointer to exposed function
 * ret : size of ret data
 * params : 
 *    int   : size of params
 *    char* : pointer to params data
 */
typedef int(*ExposedFunc_TypeDef)(int, char*);

/* Call & Return  Packet structure 
 * [CMD 1BYTE] [ID 1BYTE] [PSIZE 1BYTE] [PARAMS 28BYTES] [CHECKSUM 1BYTE]
 * CMD Type
 * 0 : Get target client name
 * 1 : Response client name
 *
 * 2 : Get target function count
 * 3 : Response function count
 *
 * 4 : Get target function name
 * 5 : Response function name
 *
 * 6 : Call target function
 * 7 : Response function retval
 */

typedef enum 
{
	SHELL_CMD_GetName,
	SHELL_CMD_AckName,
	SHELL_CMD_GetFuncCnt,
	SHELL_CMD_AckFuncCnt,
	SHELL_CMD_GetFuncName,
	SHELL_CMD_AckFuncName,
	SHELL_CMD_CallFunc,
	SHELL_CMD_RetFunc
} SHELL_CMD_TypeDef;

class ShellClient
{
	private:
	char MyName[24];																		/* name of local client */
	bool LastTargetNameStatus;													/* indicate getting target name whether had been acked */
	char TargetName[24];																/* name of target client */
	
	bool LastFuncCountStatus;														/* indicate whether getting func count had been acked */
	uint8_t FuncCount;																	/* count of local function */
	bool LastFuncNameStatus;														/* indicate whether getting func name had been acked */
	char RegisterFuncNames[SHELL_MAXFUNC][24];					/* name of local functions */
	ExposedFunc_TypeDef Functions[SHELL_MAXFUNC];				/* pointer to local registered function */
	
	uint8_t TargetFuncCount;														/* function count of target client */
	char TargetFuncNames[SHELL_MAXFUNC][24];						/* function name of target client */
	
	char TXBuffer[SHELL_PACKETSIZE];										/* transmission buffer */
	char RXBuffer[SHELL_PACKETSIZE];										/* receive buffer */
	
	bool LastRetStatus;																	/* indicate last return status */
	uint8_t LastParamsSize;															/* size of last return val */
	char LastParams[SHELL_MAXPARAMSSIZE];								/* last params */
	
	/* WARN : if the function had been executed, even the sending behavior is failed,
	 * the function will not be execute again ! 
	 * when failed, Recall to Handle() only cause retransmission.
	 */
	bool Executed;																			/* indicate whether the function which is in CALL command had been executed */
	int RetPSize;																				/* size of last retval */
	char Params[SHELL_MAXPARAMSSIZE];										/* last retval */
	
	public:
	
	bool Handled;																				/* indicate last command whether had been executed */
	
	ShellClient(const char *name, bool(*sendPacket)(char *buff));		/* create instance, init vars */
	
	bool(*SendPacket)(char *buff);											/* call to external function in order to transmission data packet */
	
	int RegisterFunc(ExposedFunc_TypeDef proc, const char *name);		/* register a function which another client can call. */
	
	bool BeginCall(const char *fname);
	bool BeginCall(const char *fname, uint8_t params);					/* reloads */
	bool BeginCall(const char *fname, int8_t params);
	bool BeginCall(const char *fname, uint16_t params);
	bool BeginCall(const char *fname, int16_t params);
	bool BeginCall(const char *fname, uint32_t params);
	bool BeginCall(const char *fname, int32_t params);
	bool BeginCall(const char *fname, uint64_t params);
	bool BeginCall(const char *fname, int64_t params);
	bool BeginCall(const char *fname, float params);
	bool BeginCall(const char *fname, double params);
	bool BeginCall(const char *fname, int pSize, void *params); /* call a available function to another client, only can use when target function list had been filled */
	bool BeginCall(int id, int pSize, char *params);		/* call a available funtion in another client */
	bool BeginGetTargetName();													/* request target client for client name */
	bool BeginGetFuncName(int id);											/* request target client for target func name */
	bool BeginGetFuncCount();														/* request target client for count of registered func */
	
	bool GetTargetNameStatus();													/* return private var : LastTargetNameStatus */
	bool GetRetStatus();																/* get whether last calling had been executed and its retval had been received. */
	bool GetFuncCountStatus();													/* return private var : LastFuncCountStatus */
	bool GetFuncNameStatus();														/* return private var : LastFuncNameStatus */
	int  GetRetValue(void *params);											/* get last retval */
	uint8_t GetUINT8();																	/* get last retval */
	int8_t GetINT8();																		/* get last retval */
	uint16_t GetUINT16();																/* get last retval */
	int16_t GetINT16();																	/* get last retval */
	uint32_t GetUINT32();																/* get last retval */
	int32_t GetINT32();																	/* get last retval */
	uint64_t GetUINT64();																/* get last retval */
	int64_t GetINT64();																	/* get last retval */
	float GetFloat();																		/* get last retval */
	double GetDouble();																	/* get last retval */
	int  GetTargetFuncCount();													/* get count of target registered function */
	int  GetTargetName(char *str);											/* get name of target client */
	void GetTargetFuncName(int id, char *str);					/* get name of target function */
	void ClearFuncCountStatus();												/* clear private var : LastFuncCountStatus */
	void ClearTargetNameStatus();												/* clear private var : LastTargetNameStatus */
	void ClearFuncNameStatus();													/* clear private var : LastFuncNameStatus */
	void ClearRetStatus();															/* clear last retval status */
	
	void PacketReceived(char *buff);										/* when receive data packet, call this function to handle it */
	bool Handle();																			/* handle a last packet */
	
};

#endif


