#ifndef __BVTYPE_H
#define __BVTYPE_H

#include <stdint.h>

typedef enum BvType {
    BvType_BIN,
    BvType_RGB565,
    BvType_RGB888,
    BvType_RGB232
} Bv_Type;

#pragma pack (1)

typedef struct BvHeader {
    uint16_t Type;
    uint32_t FrameNum;
    uint16_t Width;
    uint16_t Height;
} BvHeader_TypeDef;

#pragma pack ()

#endif
