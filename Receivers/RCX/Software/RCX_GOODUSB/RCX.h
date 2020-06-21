#ifndef __RCX_H
#define __RCX_H

#include "RCX_Type.h"
#include "RCX_Handshake.h"

#include "NRF.h"

#if (RCX_USE_CH_DEF == 1)
#  include "RCX_ChannelDef.h"
#endif

namespace RCX
{

/*Channel Manager*/
void      ChannelSetAttachEnable(bool en);
bool      ChannelGetAttachEnable();
void      ChannelSetAttach(uint8_t ch, int16_t* pValue);
int16_t*  ChannelGetAttached(uint8_t ch);
int16_t   ChannelGetAttachedIndex(int16_t* pValue);
void      ChannelSetReverse(uint8_t ch, bool en);
bool      ChannelGetReverse(uint8_t ch);
void      ChannelWrite(uint8_t ch, int16_t value);
int16_t   ChannelRead(uint8_t ch);
void      ChannelUpdate();
void      ChannelReset();

/*TxPackage Loader*/
void     TxSetObjectType(uint8_t objType);
void     TxSetObjectID(uint8_t objID);
void     TxSetPackKey(uint8_t keyVal);
uint8_t  TxGetObjectType();
uint8_t  TxGetObjectID();
int16_t* TxGetPackChannel();
void     TxLoadPack(void* txbuff);

/*RxPackage Loader*/
uint8_t  RxGetPackKey();
int16_t  RxGetPackChannel(uint8_t channel);
uint8_t  RxGetPackErrorCode();
bool     RxGetConnected();
bool     RxGetSignalLost();
void     RxSetTimeUpdate();
bool     RxLoadPack(void* rxbuff);

enum RX_ERROR_CODE{
    EC_CONNECT_TIMEOUT = 1,
    EC_TYPE_ERROR      = 1 << 1,
    EC_ID_ERROR        = 1 << 2,
    EC_CRC_ERROR       = 1 << 3,
};


/*Passback Loader*/
void    PassbaskSetBatteryInfo(int16_t voltage, int16_t level);
void    PassbaskSetUserData(void* buffer, uint16_t size);

/*Handshake*/
namespace Handshake
{

void Init(
    NRF_TRM* trm, NRF_FHSS* fhss,
    const char* name = RCX_NAME
);
uint8_t       GetSlaveID();
    
#if (RCX_IS_MASTER == 1)
const Pack_t* GetSlave(uint8_t index);
uint8_t       GetSlaveSelectIndex();
Pack_t*       GetMaster();
uint8_t       Process(uint8_t state, uint8_t slaveSelect = 0, RCX::Handshake::CMD_Type cmd = RCX::Handshake::CMD_None);
#else
bool          GetPassBack_Enable();
bool          GetFHSS_Enable();
bool          Process();
#endif
}

}


#endif
