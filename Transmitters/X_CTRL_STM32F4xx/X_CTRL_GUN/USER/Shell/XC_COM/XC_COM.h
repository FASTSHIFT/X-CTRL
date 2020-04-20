#ifndef __XC_COM_H
#define __XC_COM_H

#include "CRC32.h"
#include "stdlib.h"

void XC_COM_SetRetryDelay(uint16_t retry, uint16_t delay);
bool XC_COM_SendFile(const char* filePath);
bool XC_COM_RecvFile(const char* filePath = NULL);

#endif
