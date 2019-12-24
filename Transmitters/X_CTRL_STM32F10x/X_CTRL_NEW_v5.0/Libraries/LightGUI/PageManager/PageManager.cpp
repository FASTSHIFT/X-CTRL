#include "PageManager.h"

#define IS_PAGE(page)   (page<MaxPage)
#define IS_EVENT(event) (event<MaxEvent)

/**
  * @brief  初始化页面调度器
  * @param  pageMax: 页面最大数量
  * @param  eventMax: 事件最大数量
  * @retval 无
  */
PageManager::PageManager(uint8_t pageMax, uint8_t eventMax)
{
    MaxPage = pageMax;
    MaxEvent = eventMax;
    NowPage = 0;
    LastPage = 0xFF;

    /* 申请内存，清空列表 */
    PageList = new PageList_TypeDef[MaxPage];
    for(uint8_t page = 0; page < MaxPage; page++)
    {
        PageList[page].EventList = new CallbackFunction_t[MaxEvent];
        PageClear(page);
    }
}

/**
  * @brief  清除一个页面
  * @param  pageID: 页面编号
  * @retval true:成功 false:失败
  */
bool PageManager::PageClear(uint8_t pageID)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = 0;
    PageList[pageID].LoopCallback = 0;
    PageList[pageID].ExitCallback = 0;
    for(uint8_t e = 0; e < MaxEvent; e++)
    {
        PageList[pageID].EventList[e] = 0;
    }
    return true;
}

/**
  * @brief  注册一个基本页面，包含一个初始化函数，循环函数，退出函数
  * @param  pageID: 页面编号
  * @param  setupCallback: 初始化函数回调
  * @param  loopCallback: 循环函数回调
  * @param  exitCallback: 退出函数回调
  * @retval true:成功 false:失败
  */
bool PageManager::PageRegister_Basic(uint8_t pageID,
                                     CallbackFunction_t setupCallback,
                                     CallbackFunction_t loopCallback,
                                     CallbackFunction_t exitCallback)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = setupCallback;
    PageList[pageID].LoopCallback = loopCallback;
    PageList[pageID].ExitCallback = exitCallback;
    return true;
}

/**
  * @brief  往一个页面注册一个事件
  * @param  pageID: 页面编号
  * @param  eventID: 事件编号
  * @param  eventCallback: 事件函数回调
  * @retval true:成功 false:失败
  */
bool PageManager::PageRegister_Event(uint8_t pageID, uint8_t eventID, CallbackFunction_t eventCallback)
{
    if(!IS_PAGE(pageID))
        return false;

    if(!IS_EVENT(eventID))
        return false;

    PageList[pageID].EventList[eventID] = eventCallback;
    return true;
}

/**
  * @brief  页面事件传递
  * @param  eventID: 事件编号
  * @retval 无
  */
void PageManager::PageEventTransmit(uint8_t eventID)
{
    if(PageList[NowPage].EventList[eventID])
        PageList[NowPage].EventList[eventID]();
}

/**
  * @brief  切换到指定页面
  * @param  pageID: 页面编号
  * @retval 无
  */
void PageManager::PageChangeTo(uint8_t pageID)
{
    if(!IsPageBusy)
    {
        if(IS_PAGE(pageID))
            NowPage = pageID;

        IsPageBusy = true;
    }
}

/**
  * @brief  页面调度器状态机
  * @param  无
  * @retval 无
  */
void PageManager::Running()
{
    if(NowPage != LastPage)
    {
        IsPageBusy = true;
        NextPage = NowPage;

        if(PageList[LastPage].ExitCallback && IS_PAGE(LastPage))
            PageList[LastPage].ExitCallback();

        if(PageList[NowPage].SetupCallback && IS_PAGE(NowPage))
            PageList[NowPage].SetupCallback();

        LastPage = NowPage;
    }
    else
    {
        IsPageBusy = false;
        if(PageList[NowPage].LoopCallback && IS_PAGE(NowPage))
            PageList[NowPage].LoopCallback();
    }
}
