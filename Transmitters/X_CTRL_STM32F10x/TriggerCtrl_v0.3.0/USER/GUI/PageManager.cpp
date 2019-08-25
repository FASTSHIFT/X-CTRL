#include "PageManager.h"

PageManager::PageManager(uint8_t PageNum)
{
    MaxPage = PageNum;
    NowPage = 0;
    LastPage = 0xFF;
    if(PageList != 0) delete PageList;
    PageList = new PageList_TypeDef[MaxPage];
    for(uint8_t i = 0; i < MaxPage; i++)
    {
        PageClear(i);
    }
}

bool PageManager::PageClear(uint8_t PageNum)
{
    if(PageNum < MaxPage)
    {
        PageList[PageNum].SetupFunction = 0;
        PageList[PageNum].LoopFunction = 0;
        PageList[PageNum].ExitFunction = 0;
        PageList[PageNum].ButtonPressEventFunction = 0;
        PageList[PageNum].ButtonLongPressEventFunction = 0;
        return true;
    }
    else
    {
        return false;
    }
}

bool PageManager::PageRegister_Basic(uint8_t PageNum, 
		PageCallbackFunction_t SetupFunc,
		PageCallbackFunction_t LoopFunc,
		PageCallbackFunction_t ExitFunc)
{
    if(PageNum < MaxPage)
    {
        PageList[PageNum].SetupFunction = SetupFunc;
		PageList[PageNum].LoopFunction = LoopFunc;
		PageList[PageNum].ExitFunction = ExitFunc;
        return true;
    }
    else
    {
        return false;
    }
}

bool PageManager::PageRegister_ButtonPressEvent(uint8_t PageNum, PageCallbackFunction_t Function)
{
    if(PageNum < MaxPage)
    {
        PageList[PageNum].ButtonPressEventFunction = Function;
        return true;
    }
    else
    {
        return false;
    }
}

bool PageManager::PageRegister_ButtonLongPressEvent(uint8_t PageNum, PageCallbackFunction_t Function)
{
    if(PageNum < MaxPage)
    {
        PageList[PageNum].ButtonLongPressEventFunction = Function;
        return true;
    }
    else
    {
        return false;
    }
}

bool PageManager::PageRegister_EncoderEvent(uint8_t PageNum, PageCallbackFunction_t Function)
{
    if(PageNum < MaxPage)
    {
        PageList[PageNum].EncoderEventFunction = Function;
        return true;
    }
    else
    {
        return false;
    }
}

void PageManager::PageButtonPressEvent()
{
    if(PageList[NowPage].ButtonPressEventFunction)
        PageList[NowPage].ButtonPressEventFunction();
}

void PageManager::PageButtonLongPressEvent()
{
    if(PageList[NowPage].ButtonLongPressEventFunction)
        PageList[NowPage].ButtonLongPressEventFunction();
}

void PageManager::PageEncoderEvent()
{
    if(PageList[NowPage].EncoderEventFunction)
        PageList[NowPage].EncoderEventFunction();
}

void PageManager::PageChangeTo(uint8_t PageNum)
{
    if(!IsPageBusy)
    {
        if(PageNum < MaxPage)NowPage = PageNum;
        IsPageBusy = true;
    }
}

void PageManager::Running()
{
    if(NowPage != LastPage)
    {
        IsPageBusy = true;
        if(PageList[LastPage].ExitFunction && LastPage < MaxPage)
            PageList[LastPage].ExitFunction();
        if(PageList[NowPage].SetupFunction && NowPage < MaxPage)
            PageList[NowPage].SetupFunction();
        LastPage = NowPage;
    }
    else
    {
        IsPageBusy = false;
        if(PageList[NowPage].LoopFunction && NowPage < MaxPage)
            PageList[NowPage].LoopFunction();
    }
}
