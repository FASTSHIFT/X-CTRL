#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "FileSystem.h"
#include "SoftwareSPI.h"

SoftwareSPIClass swSPI(SD_SCK_Pin, SD_MOSI_Pin, SD_MISO_Pin);
SdFat SD;

static bool Is_SdReady = false;

bool SD_GetReady()
{
    return Is_SdReady;
}

bool Init_SD()
{
    DEBUG_FUNC();
//    SdTest_Setup();
//    while(1)SdTest_Loop();
    
#ifdef SD_CD_Pin
    pinMode(SD_CD_Pin, INPUT_PULLUP);
    if(digitalRead(SD_CD_Pin))
    {
        Is_SdReady = false;
        return Is_SdReady;
    }
#endif
      
    Debug_SERIAL.begin(115200);
    Debug_SERIAL.println("\r\nInitializing SD card...");
    if (!SD.begin(SD_CS_Pin, SD_SCK_MHZ(50)))
    {
        Debug_SERIAL.println("Card failed, or not present");
        Is_SdReady = false;
    }
    else
    {
        Debug_SERIAL.println("card initialized.");
        Is_SdReady = true;
    }

    return Is_SdReady;
}

static uint8_t SD_InsertHandle(bool isInsert)
{
    bool BvReady = false;
    if(isInsert)
    {
        BuzzMusic(MC_Type::MC_DeviceInsert);
        if(Init_SD())
            BvReady = Init_BvPlayer();
        
        if(Is_SdReady && BvReady)
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
    
    return (Is_SdReady + BvReady);
}

void Thread_SD_Monitor()
{
#ifdef SD_CD_Pin
    bool IsSDInsert = !digitalRead(SD_CD_Pin);
    __ValueMonitor(
        IsSDInsert,
        SD_InsertHandle(IsSDInsert)
    );
#endif
}
