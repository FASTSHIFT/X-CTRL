#include "PageX_Manager.h"
#include "FileGroup.h"

static uint8_t NowPage = PAGE_CtrlInfo, LastPage = 0xFF;
static bool IsPageBusy = false;

void PageSetup_x(uint8_t PageNum)
{
    switch(PageNum)
    {
    case PAGE_MainMenu:
        PageSetup_MainMenu();
        break;
    case PAGE_CtrlInfo:
        PageSetup_CtrlInfo();
        break;
    }
}

void PageExit_x(uint8_t PageNum)
{
    switch(PageNum)
    {
    case PAGE_MainMenu:
        PageExit_MainMenu();
        break;
    case PAGE_CtrlInfo:
        PageExit_CtrlInfo();
        break;
    }
}

void PageLoop_x()
{
    switch(NowPage)
    {
    case PAGE_MainMenu:
        PageLoop_MainMenu();
        break;
    case PAGE_CtrlInfo:
        PageLoop_CtrlInfo();
        break;
    }
}

void PageBottonPressEvent_x()
{
    switch(NowPage)
    {
    case PAGE_MainMenu:
        PageBottonPressEvent_MainMenu();
        break;
    case PAGE_CtrlInfo:
        PageBottonPressEvent_CtrlInfo();
        break;
    }
}

void PageBottonLongPressEvent_x()
{
    switch(NowPage)
    {
    case PAGE_MainMenu:
        PageBottonLongPressEvent_MainMenu();
        break;
    case PAGE_CtrlInfo:
        PageBottonLongPressEvent_CtrlInfo();
        break;
    }
}


void PageManager_Running()
{
    if(NowPage != LastPage)
    {
        IsPageBusy = true;
        PageExit_x(LastPage);
        PageSetup_x(NowPage);
        LastPage = NowPage;
    }
    else
    {
        IsPageBusy = false;
        PageLoop_x();
    }
}

void PageChangeTo_x(PageNum_TypeDef PageNum)
{
    if(!IsPageBusy)
    {
        NowPage = PageNum;
        IsPageBusy = true;
    }
}
