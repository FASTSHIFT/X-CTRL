#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
uint8_t NRF_TxBuff[32] = {0};
uint8_t NRF_RxBuff[32] = {0};

#define NRF_HEAD 0x83
#define PIXEL_TRAN_MAX 30

#pragma pack (1)
typedef struct {
    uint8_t Head;
    uint8_t Pos;
    uint8_t Data[PIXEL_TRAN_MAX];
} Pixels_TypeDef;

typedef struct {
    uint8_t Head;
    uint8_t ReqPos;
} PixelReq_TypeDef;
#pragma pack ()

Pixels_TypeDef* PixelsData = (Pixels_TypeDef*)&NRF_TxBuff;
PixelReq_TypeDef* PixelReq = (PixelReq_TypeDef*)&NRF_RxBuff;

static void NRF_ClearBuffer()
{
    for(int i = 0; i < sizeof(NRF_TxBuff); i++)
    {
        NRF_TxBuff[i] = NRF_RxBuff[i] = 0;
    }
}

bool Init_NRF()
{
    nrf.SetAddress(12, 34, 56, 78, 90);
    nrf.SetFreqency(40);
    nrf.SetSpeed(nrf.SPEED_2Mbps);
    nrf.init(nrf.RXTX_MODE, NRF_TxBuff, sizeof(NRF_TxBuff), 0);
    return(nrf.IsDetect());
}

void NRF_SendPixels(uint8_t *buf, uint16_t size)
{
    NRF_ClearBuffer();
    if(PixelReq->Head == NRF_HEAD)
    {
        PixelsData->Head = NRF_HEAD;
        while(1)
        {
            uint16_t offset = PIXEL_TRAN_MAX * PixelReq->ReqPos;
            
            if(offset >= size)
                return;
            
            memcpy(PixelsData->Data, buf + offset, PIXEL_TRAN_MAX);
            while(1)
            {
                nrf.RecvTran(NRF_RxBuff, NRF_TxBuff);
            }
            
        }
    }
}
