#include "XC_COM.h"

#ifdef ARDUINO
#  include "HardwareSerial.h"
#  include "Basic/TasksManage.h"
#  define XC_COM_DELAY(ms)           vTaskDelay(ms)
#  define XC_COM_BUF_WRITE(buf,size) Serial.write(buf,size)
#  define XC_COM_BUF_AVALIABLE()     Serial.available()
#  define XC_COM_BUF_READ(buf,size)  Serial.readBytes((uint8_t*)buf,size)
#  define XC_DEBUG(format, ...)
#  define XC_COM_BUF_FLUSH()         Serial.flush()
#endif

#ifdef _WIN32
#  include <windows.h>
#  include <time.h>
#  include "../FileGroup.h"
#  define  XC_COM_DELAY(ms)          Sleep(ms)
#  define XC_COM_BUF_WRITE(buf,size) Serial.write(buf,size)
#  define XC_COM_BUF_AVALIABLE()     Serial.available()
#  define XC_COM_BUF_READ(buf,size)  Serial.read(buf,size)
#  define XC_COM_BUF_FLUSH()         Serial.flush()
#  define XC_DEBUG(format, ...)      printf(format, ##__VA_ARGS__)
#  define millis()                   clock()
#endif

#define XC_HEAD_SLAVE           0x55AA
#define XC_HEAD_MASTER          0xAA55
#define XC_BUFFER_SIZE          64
#define XC_SEND_DELAY           50
#define XC_SEND_RETRY           100
#define XC_FILEPATH_MAX         64
#define XC_STRUCT_INIT(Struct)  memset(&Struct,0,sizeof(Struct))
#define XC_STRUCT_CRC32(Struct) CRC32_Calculate(0, &Struct, sizeof(Struct) - sizeof(uint32_t))

typedef enum
{
    XC_CMD_None,
    XC_CMD_Send,
    XC_CMD_Recv,
} XC_CMD_Type;

#pragma pack (1)
typedef struct
{
    uint16_t Head;
    uint8_t CMD;
    uint32_t FileSize;
    char     FileName[64];
    uint32_t PackSize;
    uint32_t PackCount;
    uint16_t PackTimeout;
//    uint8_t Reserved[64];
    uint32_t CRC32;
} XC_HandshakeHead_TypeDef;

typedef struct{
    uint32_t Size;
    uint32_t Cnt;
    uint32_t CRC32;
}XC_PackCheck_TypeDef;

typedef struct
{
    uint32_t Size;
    uint32_t Cnt;
    uint8_t Buffer[XC_BUFFER_SIZE];
    uint32_t CRC32;
} XC_DataPack_TypeDef;;
#pragma pack (0)

static FILE* pfile = NULL;
static uint16_t XC_SendRetry = XC_SEND_RETRY;
static uint16_t XC_SendDelay = XC_SEND_DELAY;
static uint32_t XC_COM_LoopTime = 0;

XC_HandshakeHead_TypeDef XC_Handshake;
XC_DataPack_TypeDef XC_DataPack;

uint32_t XC_COM_GetLoopTime()
{
    return XC_COM_LoopTime;
}

float XC_COM_GetSpeedBps()
{
    if (XC_COM_LoopTime == 0)
    {
        return 0;
    }

    return ((float)XC_Handshake.FileSize / (XC_COM_LoopTime / 1000));
}

void XC_COM_SetRetryDelay(uint16_t retry, uint16_t delay)
{
    XC_SendRetry = retry;
    XC_SendDelay = delay;
}

static uint32_t XC_GetPackCount(uint32_t fileSize, uint32_t packSize)
{
    return (fileSize / packSize + ((fileSize % packSize) == 0 ? 0 : 1));
}

static bool XC_COM_SendPrepare(const char* filePath)
{
    bool retval = false;
    pfile = fopen(filePath, "rb");
    if (pfile)
    {
        fseek(pfile, 0, SEEK_END);
        String name = String(filePath);
        int index = name.lastIndexOf('/') + 1;
        name = name.substring(index);

        XC_STRUCT_INIT(XC_Handshake);
        XC_Handshake.Head = XC_HEAD_MASTER;
        XC_Handshake.CMD = XC_CMD_Send;
        XC_Handshake.FileSize = ftell(pfile);
        strcpy(XC_Handshake.FileName, name.c_str());
        XC_Handshake.PackSize = XC_BUFFER_SIZE;
        XC_Handshake.PackCount = XC_GetPackCount(XC_Handshake.FileSize, XC_Handshake.PackSize);
        XC_Handshake.PackTimeout = XC_SendDelay;
        XC_Handshake.CRC32 = XC_STRUCT_CRC32(XC_Handshake);

        fseek(pfile, 0, SEEK_SET);
        retval = true;
    }
    return retval;
}

static bool XC_COM_SendHandshake()
{
    if (!pfile)
        return false;

    uint16_t retryCnt = XC_SendRetry;
    XC_HandshakeHead_TypeDef slave;
    
    while (retryCnt--)
    {
        XC_COM_BUF_FLUSH();
        XC_COM_BUF_WRITE(&XC_Handshake, sizeof(XC_Handshake));
        XC_COM_DELAY(XC_SEND_DELAY);

        if (XC_COM_BUF_AVALIABLE() >= sizeof(slave))
        {
            XC_STRUCT_INIT(slave);
            XC_COM_BUF_READ(&slave, sizeof(slave));
            XC_COM_BUF_FLUSH();

            if (slave.Head == XC_HEAD_SLAVE && slave.CMD == XC_CMD_Recv)
            {
                slave.Head = XC_Handshake.Head;
                slave.CMD = XC_Handshake.CMD;
                if (memcmp(&slave, &XC_Handshake, sizeof(slave)) == 0)
                {
                    return true;
                }
            }
            else
            {
                XC_DEBUG("Handshake faild, retry...\r\n");
            }
        }
    }

    fclose(pfile);
    pfile = NULL;
    return false;
}

bool XC_COM_SendFile(const char* filePath)
{
    XC_DEBUG("Send Preparing...\r\n");
    if (!XC_COM_SendPrepare(filePath))
        return false;

    XC_DEBUG(".Head = 0x%x\r\n", XC_Handshake.Head);
    XC_DEBUG(".CMD = %d\r\n", XC_Handshake.CMD);
    XC_DEBUG(".FileSize = %d Bytes\r\n", XC_Handshake.FileSize);
    XC_DEBUG(".FileName = %s\r\n", XC_Handshake.FileName);
    XC_DEBUG(".PackSize = %d\r\n", XC_Handshake.PackSize);
    XC_DEBUG(".PackCount = %d\r\n", XC_Handshake.PackCount);
    XC_DEBUG(".PackTimeout = %d\r\n", XC_Handshake.PackTimeout);
    XC_DEBUG(".CRC32 = 0x%x\r\n", XC_Handshake.CRC32);

    XC_DEBUG("Send Handshake...\r\n");
    if (!XC_COM_SendHandshake())
        return false;

    fseek(pfile, 0, SEEK_SET);

    XC_DEBUG("Sending...\r\n");

    XC_PackCheck_TypeDef packCheck;

    uint32_t count = XC_Handshake.PackCount;
    uint32_t startTime = millis();

    for (uint32_t i = 0; i < count; i++)
    {
        bool isTimeout = true;
        uint32_t readSize = fread(XC_DataPack.Buffer, 1, sizeof(XC_DataPack.Buffer), pfile);
        XC_DataPack.Size = readSize;
        XC_DataPack.Cnt = i;
        XC_DataPack.CRC32 = XC_STRUCT_CRC32(XC_DataPack);

        uint16_t retryCnt = 0;
    Retry:
        XC_COM_BUF_WRITE(&XC_DataPack, sizeof(XC_DataPack));
        uint32_t start = millis();
        while (millis() - start < XC_SendDelay)
        {
            if (XC_COM_BUF_AVALIABLE() >= sizeof(packCheck))
            {
                XC_STRUCT_INIT(packCheck);
                XC_COM_BUF_READ(&packCheck, sizeof(packCheck));
                XC_COM_BUF_FLUSH();
                if (   packCheck.Cnt == XC_DataPack.Cnt
                    && packCheck.Size == XC_DataPack.Size
                    && packCheck.CRC32 == XC_DataPack.CRC32
                    )
                {
                    //XC_DEBUG("%d/%d\r\n", XC_DataPack.Cnt + 1, XC_Handshake.PackCount);
                    //XC_DEBUG("Size:%d, CRC:0x%x\r\n", XC_DataPack.Size, packCheck.CRC32);
                    isTimeout = false;
                    break;
                }
            }
        }
        if (isTimeout)
        {
            if (retryCnt < XC_SEND_RETRY)
            {
                XC_DEBUG("Send error retry(%d)\r\n", retryCnt);
                retryCnt++;
                goto Retry;
            } 
            goto failed;
        }

        static uint32_t lastPrintTime;
        if (millis() - lastPrintTime > 1000)
        {
            lastPrintTime = millis();
            uint32_t now = XC_DataPack.Cnt + 1;
            uint32_t sum = XC_Handshake.PackCount;
            XC_DEBUG("Pack: %d/%d (%0.2f%%)\r\n", now, sum, (float)now / sum * 100);
        }
    }
    XC_COM_LoopTime = millis() - startTime;


    fclose(pfile);
    pfile = NULL;
    return true;

failed:
    return false;
}

static bool XC_COM_RecvPrepare(const char* filePath)
{
    XC_COM_BUF_FLUSH();

    uint16_t retryCnt = XC_SEND_RETRY;
    XC_DEBUG("Wait Handshake...\r\n");
    while (XC_COM_BUF_AVALIABLE() < sizeof(XC_Handshake) && --retryCnt)
    {
        XC_COM_BUF_FLUSH();
        XC_COM_DELAY(XC_SendDelay);
    }

    if (retryCnt == 0)
    {
        XC_DEBUG("time out\r\n");
        return false;
    }

    XC_DEBUG("Handshake checking...\r\n");
    XC_STRUCT_INIT(XC_Handshake);
    XC_COM_BUF_READ(&XC_Handshake, sizeof(XC_Handshake));
    XC_COM_BUF_FLUSH();

    XC_DEBUG(".Head = 0x%x\r\n", XC_Handshake.Head);
    XC_DEBUG(".CMD = %d\r\n", XC_Handshake.CMD);
    XC_DEBUG(".FileSize = %d Bytes\r\n", XC_Handshake.FileSize);
    XC_DEBUG(".FileName = %s\r\n", XC_Handshake.FileName);
    XC_DEBUG(".PackSize = %d\r\n", XC_Handshake.PackSize);
    XC_DEBUG(".PackCount = %d\r\n", XC_Handshake.PackCount);
    XC_DEBUG(".PackTimeout = %d\r\n", XC_Handshake.PackTimeout);
    XC_DEBUG(".CRC32 = 0x%x\r\n", XC_Handshake.CRC32);

    if (XC_Handshake.CRC32 != XC_STRUCT_CRC32(XC_Handshake))
    {
        XC_DEBUG("CRC(0x%x) check error\r\n", XC_Handshake.CRC32);
        return false;
    }

    XC_Handshake.Head = XC_HEAD_SLAVE;
    XC_Handshake.CMD = XC_CMD_Recv;
    XC_COM_BUF_WRITE(&XC_Handshake, sizeof(XC_Handshake));

    char FilePath[XC_FILEPATH_MAX];
    if (filePath)
    {
        sprintf(FilePath, "%s/%s", filePath, XC_Handshake.FileName);
    }
    pfile = fopen(filePath ? FilePath : XC_Handshake.FileName, "wb+");
    if (pfile)
    {
        XC_DEBUG("file %s open success\r\n", XC_Handshake.FileName);
    }
    else
    {
        XC_DEBUG("file open failed\r\n");
        return false;
    }

    return true;
}

bool XC_COM_RecvFile(const char* filePath)
{
    bool retval = false;

    if (!XC_COM_RecvPrepare(filePath))
        return false;

    XC_COM_BUF_FLUSH();
    fseek(pfile, 0, SEEK_SET);

    XC_PackCheck_TypeDef packCheck;

    uint32_t count = XC_Handshake.PackCount;
    uint32_t lastRxTime = millis();
    uint32_t lastCnt = 0xFFFFFFFF;
    
    for(uint32_t i = 0; i < count; )
    {
        if (XC_COM_BUF_AVALIABLE() >= sizeof(XC_DataPack))
        {
            lastRxTime = millis();
            XC_STRUCT_INIT(packCheck);
            XC_STRUCT_INIT(XC_DataPack);
            XC_COM_BUF_READ(&XC_DataPack, sizeof(XC_DataPack));
            XC_COM_BUF_FLUSH();
            packCheck.Cnt = i;
            packCheck.Size = XC_DataPack.Size;
            packCheck.CRC32 = XC_STRUCT_CRC32(XC_DataPack);
            XC_COM_BUF_WRITE(&packCheck, sizeof(packCheck));

            if(packCheck.CRC32 == XC_DataPack.CRC32)
            {
                if (XC_DataPack.Cnt != lastCnt)
                {
                    lastCnt = XC_DataPack.Cnt;
                    XC_DEBUG("Pack %d/%d, Size:%d, CRC:0x%x\r\n", XC_DataPack.Cnt + 1, XC_Handshake.PackCount, XC_DataPack.Size, packCheck.CRC32);
                    fwrite(XC_DataPack.Buffer, XC_DataPack.Size, 1, pfile);
                    i++;
                }
            }
            else
            {
                XC_DEBUG("CRC:0x%x ERROR! Retrying...", XC_DataPack.CRC32);
                retval = false;
            }
            retval = true;
        }

        if (millis() - lastRxTime > (XC_Handshake.PackTimeout * 2))
        {
            XC_DEBUG("Pack time out\r\n");
            retval = false;
            break;
        }
    }

    fclose(pfile);
    pfile = NULL;
    return retval;
}
