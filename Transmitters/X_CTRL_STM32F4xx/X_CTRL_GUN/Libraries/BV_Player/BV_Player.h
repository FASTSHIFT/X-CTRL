#ifndef __BV_PLAYER_H
#define __BV_PLAYER_H

#include "BvType.h"
#include "SdFat.h"

class BV_Player
{
    typedef void(*DrawPic_16bit_CallbackFunction_t)(int16_t, int16_t, uint16_t*, int16_t, int16_t);
    typedef void(*DrawPic_8bit_CallbackFunction_t)(int16_t, int16_t, uint8_t*, int16_t, int16_t);
public:
    DrawPic_16bit_CallbackFunction_t DrawPic_16bit;
    DrawPic_8bit_CallbackFunction_t  DrawPic_8bit;

    int16_t BaseX, BaseY;
    BvHeader_TypeDef Head;
    uint32_t StartFrame;
    uint32_t NowFrame;
    uint32_t EndFrame;

    typedef enum {Close, Ready, Run, Pause, Stop, Always} State_t;

    BV_Player(uint8_t *buffer, uint32_t bufferSizeMax, int16_t baseX = 0, int16_t baseY = 0);

    bool OpenVideo(String path);
    inline bool OpenVideo(const char * path)
    {
        return OpenVideo(String(path));
    }
    void CloseVideo();

    void SetPlayerState(State_t state);
    void UpdatePlayerState();
    bool IsEnd()
    {
        return(PlayerState == Close || PlayerState == Stop);
    }
    operator State_t()
    {
        return PlayerState;
    }

private:
    File BvFile;

    uint8_t* Buffer;
    uint32_t BufferSizeMax;
    uint32_t FrameSize;
    uint32_t FrameTime;

    State_t PlayerState;
    State_t PlayerStateSet;

    uint32_t PlayVideo_RGB565();
    uint32_t PlayVideo_RGB232();
    uint32_t PlayVideo_BIN();

    void DrawOneFrame();
};

#endif
