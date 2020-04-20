#include "Basic/FileGroup.h"
#include "ComPrivate.h"

typedef void(*Passback_CallbackFunction_t)(uint8_t);

typedef struct{
    uint8_t type;
    Passback_CallbackFunction_t func;
}PassbackCallback_t;

static PassbackCallback_t PassbackCallback_Grp[] = {
    {0x36, Passback_XBox360Sim}
};

void Com_PassbackProcess(uint8_t state)
{
    for(int i = 0; i < __Sizeof(PassbackCallback_Grp); i++)
    {
        if(RCX::GetTxObjectType() == PassbackCallback_Grp[i].type)
        {
            PassbackCallback_Grp[i].func(state);
            break;
        }
    }
}
