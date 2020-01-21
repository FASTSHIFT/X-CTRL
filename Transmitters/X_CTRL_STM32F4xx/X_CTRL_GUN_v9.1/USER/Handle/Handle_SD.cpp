#include "FileGroup.h"
#include "GUI_Private.h"
#include "FileSystem.h"
#include "SoftwareSPI.h"

extern void SdInfo_Setup();
extern void SdInfo_Loop();

SoftwareSPIClass swSPI(SD_SCK_Pin, SD_MOSI_Pin, SD_MISO_Pin);
SdFat SD;

bool State_SD_Enable = false;

bool Init_SD()
{
    DEBUG_FUNC();
//    SdTest_Setup();
//    while(1)SdTest_Loop();
    
#ifdef SD_CD_Pin
    pinMode(SD_CD_Pin, INPUT_PULLUP);
    if(digitalRead(SD_CD_Pin))
    {
        State_SD_Enable = false;
        return State_SD_Enable;
    }
#endif
      
    Debug_SERIAL.begin(115200);
    Debug_SERIAL.println("\r\nInitializing SD card...");
    if (!SD.begin(SD_CS_Pin, SD_SCK_MHZ(50)))
    {
        Debug_SERIAL.println("Card failed, or not present");
        State_SD_Enable = false;
    }
    else
    {
        Debug_SERIAL.println("card initialized.");
        State_SD_Enable = true;
    }

    return State_SD_Enable;
}

static uint8_t SDInsert_Handle(bool isInsert)
{
    if(isInsert)
    {
        BuzzMusic(MC_Type::MC_DeviceInsert);
        if(Init_SD())
            Init_BvPlayer();
        
        if(State_SD_Enable && State_BV_Enable)
        {
            PageRegister_MainMenuDymanic(PAGE_MainMenu);
        }
        else
            PageRegister_MainMenu(PAGE_MainMenu);
    }
    else
    {
        BuzzMusic(MC_Type::MC_DevicePullout);

        PageRegister_MainMenu(PAGE_MainMenu);
    }
    
    if(page.NowPage == PAGE_MainMenu)
    {
        page.PageStackClear();
        page.OldPage = 0;
        page.PagePush(PAGE_MainMenu);
    }
    
    return (State_SD_Enable + State_BV_Enable);
}

void Thread_SD_Monitor()
{
#ifdef SD_CD_Pin
    bool IsSDInsert = !digitalRead(SD_CD_Pin);
    __ValueMonitor(
        IsSDInsert,
        SDInsert_Handle(IsSDInsert)
    );
#endif
}
