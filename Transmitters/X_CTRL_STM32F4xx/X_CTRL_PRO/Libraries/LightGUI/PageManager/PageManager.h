
/**********     基于简单状态机的GUI页面调度器    **********/
/**********        Designed By _VIFEXTech        **********/
/**
  * @Finished: 2018.10.6  - v1.0 拥有简单的页面切换管理，事件传递调度
  * @Upgrade:  2018.4.1   - v1.1 增加注释，重写事件传递调度，减少重复代码
  */

#ifndef __PAGEMANAGER_H
#define __PAGEMANAGER_H

#include "stdint.h"

class PageManager {
    typedef void(*CallbackFunction_t)(void);
    typedef void(*EventFunction_t)(int,void*);
    typedef struct {
        CallbackFunction_t SetupCallback;
        CallbackFunction_t LoopCallback;
        CallbackFunction_t ExitCallback;
        EventFunction_t EventCallback;
    } PageList_TypeDef;
public:
    PageManager(uint8_t pageMax, uint8_t pageStackSize = 10);
    uint8_t NowPage, LastPage, NextPage;
    bool PageRegister(
        uint8_t pageID,
        CallbackFunction_t setupCallback,
        CallbackFunction_t loopCallback,
        CallbackFunction_t exitCallback,
        EventFunction_t eventCallback
    );
    bool PageClear(uint8_t pageID);
    bool PagePush(uint8_t pageID);
    bool PagePop();
    void PageChangeTo(uint8_t pageID);
    void PageEventTransmit(int event, void* param);
    void PageStackClear();
    void Running();
private:
    PageList_TypeDef* PageList;
    uint8_t *PageStack;
    uint8_t PageStackSize;
    uint8_t PageStackTop;

    uint8_t MaxPage;
    uint8_t NewPage;
    uint8_t OldPage;
    bool IsPageBusy;
};

#endif
