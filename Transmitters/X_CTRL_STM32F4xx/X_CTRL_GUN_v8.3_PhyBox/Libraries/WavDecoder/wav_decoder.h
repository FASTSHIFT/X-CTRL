
#ifndef __WAVDECODER_H__
#define __WAVDECODER_H__

#include "stdint.h"

#define WAV_ERRCODE_CannotFindDataFlag      0xf0
#define WAV_ERRCODE_EndOfFile               0xf1

#define WAV_DataFlagOffset_MAX              1024

typedef uint32_t HWAVEFILE;         /* 表示 wave 文件实例句柄类型 */

typedef uint8_t(*WAV_Stream_Interface_TypeDef)(HWAVEFILE hFile, uint8_t reqBytes, uint8_t **buffer);

#pragma pack (1)
typedef struct
{
    uint8_t  FileFlag[4];       /* 文件标识，一般是 字符串 "RIFF" (0x46464952) */
    uint32_t FileLength;        /* 文件长度，该值加上 8 即为文件大小（8 byte是因为该文件长度不包括前2个数据项，前两个数据项一共 8 byte） */
    uint8_t  FileType[4];       /* 文件格式，一般是 字符串 "WAVE" (0x45564157)*/
    uint8_t  ChunkFlag[4];      /* 格式块标识，一般是 字符串 "fmt" (0xXX746D66) */
    uint32_t ChuckLength;       /* 格式块长度，根据编码格式，一般有 16、18、20、40 */
    uint16_t EncodeType;        /* 编码格式，如果文件使用 PCM 编码，则为 1 */
    uint16_t ChannelCnt;        /* 声道个数，单声道为 1、双声道为 2 */
    uint32_t SampleFreq;        /* 采样频率，一般为 11025、22050、44100、48000 */
    uint32_t BytePerSecond;     /* 比特率，声道数 * 采样率 * 采样位数 / 8 */
    uint16_t BytePerFrame;      /* 采样帧大小，声道数 * 采样位数 / 8 */
    uint16_t SampleBits;        /* 采样位数，一般有 4、8、12、16、24、32 */
} WAV_Header_TypeDef;

typedef struct
{
    int32_t LeftVal;
    int32_t RightVal;
} WAV_DataChunk_TypeDef;

#pragma pack ()

typedef struct
{
    HWAVEFILE hWave;
    uint8_t IsEnd;
    uint32_t DataSize;
    uint32_t DataPosition;
    WAV_Header_TypeDef Header;
    WAV_DataChunk_TypeDef CurrentData;
    WAV_Stream_Interface_TypeDef StreamInterface;
} WAV_TypeDef;

HWAVEFILE   Wav_StructInit(WAV_TypeDef *pWav, WAV_Stream_Interface_TypeDef streamProc);
int         Wav_Open(WAV_TypeDef *pWav);
int         Wav_Next_16Bit2Channel(WAV_TypeDef *pWav);

#endif


