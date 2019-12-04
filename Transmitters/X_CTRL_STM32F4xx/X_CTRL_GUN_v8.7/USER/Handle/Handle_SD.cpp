#include "FileGroup.h"
#include "GUI_Private.h"
#include "FileSystem.h"
#include "SoftwareSPI.h"

SoftwareSPIClass swSPI(SD_SCK_Pin, SD_MOSI_Pin, SD_MISO_Pin);

Sd2Card card;
SdVolume volume;
SdFile root;

bool State_SD_Enable = false;

void PrintCardInfo()
{
  if (!card.init(SPI_HALF_SPEED, ChipSelect_Pin)) {
    Debug_SERIAL.println("initialization failed. Things to check:");
    Debug_SERIAL.println("* is a card inserted?");
    Debug_SERIAL.println("* is your wiring correct?");
    Debug_SERIAL.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Debug_SERIAL.println("Wiring is correct and a card is present.");
  }
    // print the type of card
  Debug_SERIAL.println();
  Debug_SERIAL.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Debug_SERIAL.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Debug_SERIAL.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Debug_SERIAL.println("SDHC");
      break;
    default:
      Debug_SERIAL.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Debug_SERIAL.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Debug_SERIAL.print("Clusters:          ");
  Debug_SERIAL.println(volume.clusterCount());
  Debug_SERIAL.print("Blocks x Cluster:  ");
  Debug_SERIAL.println(volume.blocksPerCluster());

  Debug_SERIAL.print("Total Blocks:      ");
  Debug_SERIAL.println(volume.blocksPerCluster() * volume.clusterCount());
  Debug_SERIAL.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Debug_SERIAL.print("Volume type is:    FAT");
  Debug_SERIAL.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Debug_SERIAL.print("Volume size (Kb):  ");
  Debug_SERIAL.println(volumesize);
  Debug_SERIAL.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Debug_SERIAL.println(volumesize);
  Debug_SERIAL.print("Volume size (Gb):  ");
  Debug_SERIAL.println(volumesize / 1024.0);

  Debug_SERIAL.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

bool Init_SD()
{
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
    if (!SD.begin(SPI_FULL_SPEED, ChipSelect_Pin))
    {
        Debug_SERIAL.println("Card failed, or not present");
        State_SD_Enable = false;
    }
    else
    {
        Debug_SERIAL.println("card initialized.");
        State_SD_Enable = true;
        PrintCardInfo();
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
