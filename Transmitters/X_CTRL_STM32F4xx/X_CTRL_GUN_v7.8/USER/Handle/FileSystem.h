#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

#include "SD.h"

#define ChipSelect_Pin SD_CS_Pin

extern Sd2Card card;
extern SdVolume volume;
extern SdFile root;

#define BV_BUFF_HEIGHT 130
#define BV_BUFF_WIDTH  130
extern uint8_t BV_Buffer[BV_BUFF_HEIGHT * BV_BUFF_WIDTH * sizeof(uint16_t)];

extern String BvFilePath;
extern String WavFilePath;

#endif
