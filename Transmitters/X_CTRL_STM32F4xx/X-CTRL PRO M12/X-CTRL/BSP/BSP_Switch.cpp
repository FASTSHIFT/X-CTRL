#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "BSP.h"

SwitchEvent swGrp[SW_IDX_MAX];

static void SwitchEvent_Handler(SwitchEvent* sw, int event)
{
    if(event == SwitchEvent_Type::EVENT_SwitchUp)
    {
        Audio_Tone(500, 300);
    }
    else if(event == SwitchEvent_Type::EVENT_SwitchDown)
    {
        Audio_Tone(200, 100);
    }
    
    /*传递到页面事件*/
    page.PageEventTransmit(event, sw);
}

void Switch_Init()
{
    __LoopExecute(swGrp[i].EventAttach(SwitchEvent_Handler), __Sizeof(swGrp));
}

typedef enum{
    SW_E_UP   = 0x2000,
    SW_E_DOWN = 0x1000,
    SW_F_UP   = 0x0080,
    SW_F_DOWN = 0x0040,
    SW_G_UP   = 0x8000,
    SW_G_DOWN = 0x4000,
    SW_H_UP   = 0x0010,
    SW_H_DOWN = 0x0020,
}SwitchBit_Type;

void Switch_Update(uint16_t swVal)
{
    swE.EventMonitor(swVal & SW_E_UP, swVal & SW_E_DOWN); 
    swF.EventMonitor(swVal & SW_F_UP, swVal & SW_F_DOWN); 
    swG.EventMonitor(swVal & SW_G_UP, swVal & SW_G_DOWN); 
    swH.EventMonitor(swVal & SW_H_UP, swVal & SW_H_DOWN);
    
    for(int i = 0; i < __Sizeof(swGrp); i++)
    {
        CTRL.Switch[i] = swGrp[i] * RCX_CHANNEL_DATA_MAX;
    }
}
