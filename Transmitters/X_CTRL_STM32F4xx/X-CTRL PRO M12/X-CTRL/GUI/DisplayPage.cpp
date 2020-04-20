#include "DisplayPrivate.h"

PageManager page(PAGE_MAX);

#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)

void Page_Init()
{
    PAGE_REG(HandshakeAuto);
    PAGE_REG(CtrlPage);
    
    page.PagePush(PAGE_HandshakeAuto);
}

void Page_Delay(uint32_t ms)
{
    uint32_t lastTime = millis();
    while(millis() - lastTime <= ms)
    {
        lv_task_handler();
    }
}
