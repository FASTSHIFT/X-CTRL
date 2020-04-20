#ifndef __CHANNEL_DEF_H
#define __CHANNEL_DEF_H

#define CH_LMT_L 0
#define CH_LMT_R 1
#define CH_JSL_X 4
#define CH_JSL_Y 5
#define CH_JSR_X 6
#define CH_JSR_Y 7
typedef enum
{
    BT_UP    = 1,
    BT_DOWN  = 1 << 1,
    BT_LEFT  = 1 << 2,
    BT_RIGHT = 1 << 3,
    BT_L1    = 1 << 4,
    BT_L2    = 1 << 5,
    BT_R1    = 1 << 6,
    BT_R2    = 1 << 7
} KeyValue_t;
#define BT_OK   BT_RIGHT
#define BT_BACK BT_LEFT

#endif
