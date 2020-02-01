#ifndef __RCX_H
#define __RCX_H

#include "../RCX_Type.h"
#include "../RCX_Handshake.h"
#include "SysConfig.h"

#define RCX_ChannelData_Max   1000

#ifdef TIM_Handshake
#define RCX_TIM_Handshake     TIM_Handshake
#endif

namespace RCX
{
/*Package Loader*/
void SetObjectType(uint8_t objType);
void SetObjectID(uint8_t objID);
void SetPackKey(uint8_t keyVal);
uint8_t GetObjectType();
int16_t* GetPackChannel();
void LoadPack(void* txbuff);

/*Channel Manager*/
void ChannelAttachValueSetEnable(bool en);
bool ChannelAttachValueGetEnable();
void ChannelAttachValue(uint8_t ch, int16_t* pValue);
bool ChannelGetAttached(uint8_t ch);
int16_t ChannelCheckValue(int16_t* pValue);
int16_t** ChannelGetValRegList();
void ChannelWriteValue(uint8_t ch, int16_t value);
int16_t ChannelReadValue(uint8_t ch);
void ChannelUpdate();
void ChannelReset();

/*Passbac kManager*/    
typedef void(*CustomDataPack_Callback_t)(uint8_t*);
bool PassBackAddCustom(CustomDataPack_Callback_t callback);
void PassbackProcess(uint8_t* rxbuff);
    
namespace Handshake
{
void  Init(
    NRF_TRM*  trm,
    NRF_FHSS* fhss, 
    const char* masterName = "Master"
);
const Pack_t* GetSlave(uint8_t index);
Pack_t* GetMaster();
uint8_t Process(uint8_t state, uint8_t slaveSelect = 0, RCX::Handshake::CMD_Type cmd = RCX::Handshake::CMD_None);
}

}


#endif
