#ifndef __CRC32_H
#define __CRC32_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

uint32_t CRC32_Calculate(uint32_t crc, const void *buf, size_t size);
    
#ifdef __cplusplus
}
#endif

#endif
