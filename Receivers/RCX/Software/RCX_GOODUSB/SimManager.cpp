#include "FileGroup.h"

typedef enum {
  Sim_XBox360,
  Sim_Keyboard,
  Sim_Mouse,
  Sim_MAX
} SimObj_t;

uint8_t Sim_ObjectSelect = 0;

typedef struct{
  voidFuncPtr_t SetupCallback;
  voidFuncPtr_t LoopCallback;
}SimFunc_t;

static SimFunc_t SimFunc_Grp[Sim_MAX] = {0};

bool Sim_RegFunc(uint8_t id, voidFuncPtr_t setupFunc, voidFuncPtr_t loopFunc)
{
    if(id >= Sim_MAX)
        return false;

    SimFunc_Grp[id].SetupCallback = setupFunc;
    SimFunc_Grp[id].LoopCallback = loopFunc;

    return true;
}

#define SIM_REG(name)\
do{\
    extern void name##_Reg(uint8_t id);\
    name##_Reg(Sim_##name);\
}while(0)

static void Sim_InitFuncGrp()
{
    SIM_REG(XBox360);
    SIM_REG(Keyboard);
    SIM_REG(Mouse);
}

void Sim_DeviceInit()
{
    Sim_InitFuncGrp();
    if(SimFunc_Grp[Sim_ObjectSelect].SetupCallback)
        SimFunc_Grp[Sim_ObjectSelect].SetupCallback();
}

void Sim_DeviceProcess()
{
    if(SimFunc_Grp[Sim_ObjectSelect].LoopCallback)
        SimFunc_Grp[Sim_ObjectSelect].LoopCallback();
}

void Sim_SetNextObj()
{
  Sim_ObjectSelect++;
  Sim_ObjectSelect %= Sim_MAX;
}
