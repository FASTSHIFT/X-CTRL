#ifndef __BV_PLAYER_H
#define __BV_PLAYER_H

#include "BvType.h"
#include "SD.h"

class BV_Player {
    typedef void(*FastDrawPicture_u16_CallbackFunction_t)(uint16_t*,BvHeader_TypeDef*,int16_t*,int16_t*);
    typedef void(*FastDrawPicture_u8_CallbackFunction_t)(uint8_t*,BvHeader_TypeDef*,int16_t*,int16_t*);
public:
    FastDrawPicture_u16_CallbackFunction_t CallBackDrawPicture_u16;
    FastDrawPicture_u8_CallbackFunction_t  CallBackDrawPicture_u8;

    typedef enum{Close, Ready, Run, Pause, Stop, Always}State_t;

    BV_Player(uint8_t *buffer,uint32_t bufferSizeMax, int16_t baseX = 0, int16_t baseY = 0);
    
    bool OpenVideo(String path);
    inline bool OpenVideo(const char * path)
    {
        return OpenVideo(String(path));
    }
    void CloseVideo();
    
    void SetPlayerState(State_t state);
    void UpdatePlayerState();
	operator State_t() {
        return PlayerState;
    }
    
private:
    File BvFile;
    BvHeader_TypeDef Head;

    int16_t BaseX,BaseY;

    uint8_t* Buffer;
    uint32_t BufferSize;
    uint32_t BufferSizeMax;
    
    uint32_t StartFrame;
    uint32_t NowFrame;
    uint32_t EndFrame;

    State_t PlayerState;
    State_t PlayerStateSet;
    
    uint32_t PlayVideo_RGB565();
    uint32_t PlayVideo_RGB232();
    uint32_t PlayVideo_BIN();
};

#endif
