
#include "wav_decoder.h"

#define WAV_Read(n) if ((err = pWav->StreamInterface(pWav->hWave, n, &pBuff)) != 0) { return err; }

HWAVEFILE Wav_StructInit(WAV_TypeDef *pWav, WAV_Stream_Interface_TypeDef streamInterface)
{
    static volatile HWAVEFILE handleCounter = 0;
    pWav->IsEnd = 0;
    pWav->DataSize = 0;
    pWav->DataPosition = 0;
    pWav->hWave = handleCounter ++;
    pWav->StreamInterface = streamInterface;
    pWav->CurrentData.LeftVal = pWav->CurrentData.RightVal = 0;
    return pWav->hWave;
}
int Wav_Open(WAV_TypeDef *pWav)
{
    int err = 0;
    uint8_t *pBuff = 0;
    WAV_Read(sizeof(WAV_Header_TypeDef));
    pWav->Header = *(WAV_Header_TypeDef *)pBuff;
    int wavOffset = WAV_DataFlagOffset_MAX;
    uint32_t dataFlag = 0;
    uint32_t dataSize = 0;
    while (wavOffset--)
    {
        WAV_Read(1);
        dataFlag <<= 8;
        dataFlag |= *pBuff & 0xff;
        if (dataFlag == 0x64617461)
        {
            WAV_Read(sizeof(uint32_t));
            dataSize = *(uint32_t *)pBuff;
        }
    }
    pWav->DataPosition = 0;
    pWav->DataSize = dataSize;
    if (!dataSize)
    {
        return WAV_ERRCODE_CannotFindDataFlag;
    }
    return 0;
}
int Wav_Next_16Bit2Channel(WAV_TypeDef *pWav)
{
    if (pWav->IsEnd)
    {
        return WAV_ERRCODE_EndOfFile;
    }
    int err = 0;
    uint8_t *pBuff = 0;
    WAV_Read(4);
    pWav->CurrentData.LeftVal = *(int16_t *)pBuff;
    pWav->CurrentData.RightVal = *(int16_t *)(pBuff + 2);
    pWav->DataPosition += 4;
    if (pWav->DataPosition >= pWav->DataSize)
    {
        pWav->IsEnd = 1;
    }
    return 0;
}
