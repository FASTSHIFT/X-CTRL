#include "HMI_Ctrl.h"

typedef struct HMI_Protocal{
	uint16_t DataHead;
	uint8_t Page;
	uint8_t Widget;
	uint8_t Data;
}Protocal_HMI_TypeDef;
extern Protocal_HMI_TypeDef HMI_Info;
#define DataHead_Event   (uint16_t)0xDDDD
#define DataHead_Commond (uint16_t)0xCCCC
#define Serial_HMI Serial2

Protocal_HMI_TypeDef HMI_Info;

void HMI_DataRecvEvent()
{
    Serial_HMI.readBytes((uint8_t*)&HMI_Info, sizeof(HMI_Info));
    if(HMI_Info.DataHead == DataHead_Event)
    {
    }
    else if(HMI_Info.DataHead == DataHead_Commond)
    {
    }
    Serial_HMI.flush();
}
