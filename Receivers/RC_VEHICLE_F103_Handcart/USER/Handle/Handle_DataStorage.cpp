#include "FileGroup.h"
#include "EEPROM.h"

#define STARTADDR 0x20
#define RegDataList_MAX 20

using namespace EEPROM_Chs;

typedef struct {
    uint16_t DataLength;
    uint16_t DataCntSum;
    uint32_t CheckSum;
} EEPROM_DataHead_t;

typedef struct {
    uint8_t *pData;
    uint16_t Size;
} EEPROM_RegList_t;

static EEPROM_RegList_t DataRegList[RegDataList_MAX];//用户数据列表
static uint16_t RegListOffset = 0;

static uint16_t EEPROM_ReadNext(bool reset = false, uint16_t start = STARTADDR)
{
    static uint16_t OffsetAddr = STARTADDR;
    if(reset)
        OffsetAddr = start;

    uint16_t ret = EEPROM.read(OffsetAddr);
    OffsetAddr++;
    return ret;
}

static void EEPROM_WriteNext(uint16_t data, bool reset = false, uint16_t start = STARTADDR)
{
    static uint16_t OffsetAddr = STARTADDR;
    if(reset)
        OffsetAddr = start;

    EEPROM.update(OffsetAddr, data);
    OffsetAddr++;
}

/*注册任意类型变量*/
bool EEPROM_Register(void *pdata, uint16_t size)
{
    if(RegListOffset == RegDataList_MAX)
        return false;
    
    DataRegList[RegListOffset].pData = (uint8_t*)pdata;
    DataRegList[RegListOffset].Size = size;
    RegListOffset++;
    return true;
}

/*
数据结构:
|                     帧头                       |                 用户数据
| 用户数据总长 | 用户数据总个数 | 用户数据校验和 | 数据1大小 |  数据1  | 数据2大小 |  数据2  |...
|     2Byte    |     2Byte      |     4Byte      |   2Byte   |  xByte  |   2Byte   |  xByte  |...
*/
bool EEPROM_Handle(EEPROM_Chs_t chs)
{
    bool retval = false;
    EEPROM_DataHead_t head = {0};
    if(chs == ReadData)
    {
        /*获取帧头信息*/
        ((uint16_t*)(&head))[0] = EEPROM_ReadNext(true);//DataLength，将指针指向首地址
        ((uint16_t*)(&head))[1] = EEPROM_ReadNext();//DataCntSum
        ((uint16_t*)(&head))[2] = EEPROM_ReadNext();//CheckSum(HIGH 16bit)
        ((uint16_t*)(&head))[3] = EEPROM_ReadNext();//CheckSum(LOW  16bit)

        /*获取用户数据校验和*/
        uint32_t UserDataSum = 0;
        for(uint16_t i = 0; i < head.DataLength; i++)
            UserDataSum += EEPROM_ReadNext();

        /*校验用户数据是否与注册列表匹配*/
        if(UserDataSum != head.CheckSum || head.DataCntSum != RegListOffset)
            return false;

        /*在帧头信息末地址的基础上往前移动一个地址，将下一次读取首地址指向用户数据首地址*/
        EEPROM_ReadNext(true, STARTADDR + sizeof(head) / 2 - 1);

        /*遍历注册列表*/
        for(uint16_t cnt = 0; cnt < head.DataCntSum; cnt++)
        {
            uint16_t UserSize = EEPROM_ReadNext();

            /*/校验是否与用户注册列表匹配*/
            if(UserSize != DataRegList[cnt].Size)
                return false;

            /*将读取的数据写入注册列表中pData指针指向的数据*/
            for(uint16_t offset = 0; offset < UserSize; offset++)
                (DataRegList[cnt].pData)[offset] = EEPROM_ReadNext();
        }
        retval = true;
    }
    else if(chs == SaveData)
    {
        /*生成帧头信息*/

        /*用户数据总个数*/
        head.DataCntSum = RegListOffset;

        /*遍历注册列表*/
        for(uint16_t cnt = 0; cnt < RegListOffset; cnt++)
        {
            /*用户数据总长*/
            head.DataLength += sizeof(DataRegList[cnt].Size) / 2 + DataRegList[cnt].Size;
            /*用户数据校验和*/
            head.CheckSum += DataRegList[cnt].Size;
            for(uint16_t offset = 0; offset < DataRegList[cnt].Size; offset++)
                head.CheckSum += (DataRegList[cnt].pData)[offset];
        }

        /*写入帧头信息*/
        EEPROM_WriteNext(((uint16_t*)(&head))[0], true);//DataLength，将指针指向首地址
        EEPROM_WriteNext(((uint16_t*)(&head))[1]);//DataCntSum
        EEPROM_WriteNext(((uint16_t*)(&head))[2]);//CheckSum(HIGH 16bit)
        EEPROM_WriteNext(((uint16_t*)(&head))[3]);//CheckSum(LOW  16bit)

        /*遍历注册列表，写入用户数据*/
        for(uint16_t cnt = 0; cnt < RegListOffset; cnt++)
        {
            EEPROM_WriteNext(DataRegList[cnt].Size);
            for(uint16_t offset = 0; offset < DataRegList[cnt].Size; offset++)
                EEPROM_WriteNext((DataRegList[cnt].pData)[offset]);
        }

        retval = EEPROM_Handle(ReadData);
    }
    return retval;
}
