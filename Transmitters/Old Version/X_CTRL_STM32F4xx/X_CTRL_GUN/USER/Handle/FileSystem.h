#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

#include "SdFat.h"

#define ChipSelect_Pin SD_CS_Pin

extern uint8_t BV_Buffer[];

extern String BvFilePath;
extern String WavFilePath;
extern String LuaFilePath;

#endif
