#include "BSP.h"
#include "extEEPROM/extEEPROM.h"

/*外部EEPROM*/
extEEPROM eeprom(kbits_256, 1, 64, 0x50);

/*起始储存地址*/
#define START_ADDR      0

/*最大注册变量数量*/
#define RegDataList_MAX 32

/*读取超时设置*/
#define DataReadTimeOut  5000 /*[ms]*/

/*调试日志输出接口*/
#define DS_DEBUG(data)            //Serial.println(data);
#define DS_DEBUG_FMT(format, ...) //Serial.printf(format, ##__VA_ARGS__)

/*单字节对齐*/
#pragma pack(1)
/*
---------------------------------------------数据结构--------------------------------------------
|                     帧头                       |                 用户数据
| 用户数据总长 | 用户数据总个数 | 用户数据校验和 | 数据1大小 |  数据1  | 数据2大小 |  数据2  |...
|     2Byte    |     2Byte      |     4Byte      |   2Byte   |  xByte  |   2Byte   |  xByte  |...
*/

/*帧头数据结构*/
typedef struct {
    uint16_t DataLength;/*用户数据总长*/
    uint16_t DataCntSum;/*用户数据总个数*/
    uint32_t CheckSum;/*用户数据校验和*/
} EEPROM_DataHead_t;

/*储存数据结构*/
typedef struct {
    uint8_t* pData;/*用户数据地址*/
    uint16_t Size;/*用户数据大小*/
} EEPROM_RegList_t;
#pragma pack()

/*用户数据列表*/
static EEPROM_RegList_t DataReg_List[RegDataList_MAX] = {0};

/*用户数据注册事件的个数*/
static uint16_t DataReg_Cnt = 0;

/*当前读写的地址偏移量*/
static uint16_t DataReg_OffsetAddr = 0;

/**
  * @brief  EEPROM读取接口
  * @param  addr:要读取的地址
  * @retval 这个地址保存的数据
  */
static uint8_t EEPROM_Read(uint32_t addr)
{
    DS_DEBUG_FMT(">> R: addr=%d data=0x%x\r\n", addr, eeprom.read(START_ADDR + addr));
    return eeprom.read(START_ADDR + addr);
}

/**
  * @brief  从EEPROM读取一串数据
  * @param  addr:要读取的起始地址
  * @param  pdata:接收数据的起始地址
  * @param  size:接收数据的个数
  * @retval 无
  */
static void EEPROM_Read(uint32_t addr, void* pdata, uint32_t size)
{
    for(uint32_t i = 0; i < size; i++)
    {
        ((uint8_t*)pdata)[i] = EEPROM_Read(addr + i);
    }
}

/**
  * @brief  EEPROM写数据接口
  * @param  addr:要读取的地址
  * @param  data:要写入的数据
  * @retval 无
  */
static void EEPROM_Write(uint32_t addr, uint8_t data)
{
    DS_DEBUG_FMT("<< W: addr=%d data=0x%x\r\n", addr, data);
    eeprom.update(START_ADDR + addr, data);
}

/**
  * @brief  往EEPROM写入一串数据
  * @param  addr:要写入的起始地址
  * @param  pdata:被写数据的起始地址
  * @param  size:写入数据的个数
  * @retval 无
  */
static void EEPROM_Write(uint32_t addr, void* pdata, uint32_t size)
{
    for(uint32_t i = 0; i < size; i++)
    {
        EEPROM_Write(addr + i, ((uint8_t*)pdata)[i]);
    }
}

/**
  * @brief  读取下一个地址的数据
  * @param  无
  * @retval 下一个地址储存的数据
  */
static uint8_t EEPROM_ReadNext()
{
    uint8_t data = EEPROM_Read(DataReg_OffsetAddr);
    DataReg_OffsetAddr++;
    return data;
}

/**
  * @brief  往下一个地址写数据
  * @param  data:待写入数据
  * @retval 无
  */
static void EEPROM_WriteNext(uint8_t data)
{
    EEPROM_Write(DataReg_OffsetAddr, data);
    DataReg_OffsetAddr++;
}

/**
  * @brief  EEPROM初始化
  * @param  无
  * @retval 错误代码
  */
uint8_t EEPROM_Init()
{
    DEBUG_FUNC_LOG();
    return eeprom.begin(eeprom.twiClock100kHz);
}

/**
  * @brief  注册任意类型变量
  * @param  *pdata:指向待储存数据的指针
  * @param  size:数据大小
  * @retval true成功 false失败
  */
bool EEPROM_Register(void *pdata, uint16_t size)
{
    if(DataReg_Cnt == RegDataList_MAX)
        return false;
    
    DataReg_List[DataReg_Cnt].pData = (uint8_t*)pdata;
    DataReg_List[DataReg_Cnt].Size = size;
    DataReg_Cnt++;
    return true;
}

/**
  * @brief  保存注册的所有数据
  * @param  无
  * @retval true成功 false失败
  */
bool EEPROM_SaveAll()
{
    DS_DEBUG(__FUNCTION__);
    
    /*初始化帧头*/
    EEPROM_DataHead_t head;
    memset(&head, 0, sizeof(head));
    
    /*关中断，防止数据篡改*/
    noInterrupts();
    
    /*生成帧头信息*/
    /*用户注册数据的总个数*/
    head.DataCntSum = DataReg_Cnt;

    /*遍历注册列表,生成校验信息*/
    for(uint32_t cnt = 0; cnt < DataReg_Cnt; cnt++)
    {
        /*用户数据总长*/
        uint16_t length = (sizeof(DataReg_List[cnt].Size) + DataReg_List[cnt].Size);
        
        /*用户数据校验总和*/
        uint32_t checksum = 0;
        uint16_t size = DataReg_List[cnt].Size;
        checksum += highByte(size);
        checksum += lowByte(size);
        for(uint32_t offset = 0; offset < size; offset++)
        {
            checksum += (DataReg_List[cnt].pData)[offset];
        }
        
        DS_DEBUG_FMT("CNT %d : size = %d, length = %d, checksum = %d\r\n", cnt, size, length, checksum);
        
        head.DataLength += length;
        head.CheckSum += checksum;
    }

    /*写入帧头信息*/
    EEPROM_Write(0, &head, sizeof(head));
    DS_DEBUG("Write head");
    DS_DEBUG_FMT(".DataLength   = %d\r\n", head.DataLength);
    DS_DEBUG_FMT(".DataCntSum   = %d\r\n", head.DataCntSum);
    DS_DEBUG_FMT(".DataCheckSum = %d\r\n", head.CheckSum);
    
    /*移到帧头之后,就是用户数据区*/
    DataReg_OffsetAddr = sizeof(head);

    /*遍历注册列表，写入用户数据*/
    for(uint32_t i = 0; i < DataReg_Cnt; i++)
    {
        uint16_t size = DataReg_List[i].Size;
        EEPROM_WriteNext(highByte(size));
        EEPROM_WriteNext(lowByte(size));
        for(uint32_t offset = 0; offset < size; offset++)
        {
            EEPROM_WriteNext((DataReg_List[i].pData)[offset]);
        }
    }
    
    /*判断是否写入成功*/
    bool retval =  EEPROM_ReadAll();
    
    /*重新启用中断*/
    interrupts();
    
    return retval;
}

/**
  * @brief  读取注册的所有数据
  * @param  无
  * @retval true成功 false失败
  */
bool EEPROM_ReadAll()
{
    DS_DEBUG(__FUNCTION__);
    
    /*初始化帧头*/
    EEPROM_DataHead_t head;
    memset(&head, 0, sizeof(head));
    
    /*获取帧头信息*/
    EEPROM_Read(0, &head, sizeof(head));
    DS_DEBUG("Read head");
    DS_DEBUG_FMT(".DataLength   = %d\r\n", head.DataLength);
    DS_DEBUG_FMT(".DataCntSum.  = %d\r\n", head.DataCntSum);
    DS_DEBUG_FMT(".DataCheckSum = %d\r\n", head.CheckSum);

    /*初始化用户数据校验和*/
    uint32_t UserDataCheckSum = 0;
    uint32_t StartReadTime = millis();
    
    /*移到用户数据区*/
    DataReg_OffsetAddr = sizeof(head);

    /*计算校验和*/
    for(uint16_t i = 0; i < head.DataLength; i++)
    {
        /*读取是否超时*/
        if(millis() - StartReadTime > DataReadTimeOut)
        {
            DS_DEBUG("ERROR! data read timeout");
            return false;
        }
        
        UserDataCheckSum += EEPROM_ReadNext();
    }

    /*校验用户数据是否与注册列表匹配*/
    /*检查校验和*/
    if(UserDataCheckSum != head.CheckSum)
    {
        DS_DEBUG_FMT("ERROR! UserDataCheckSum = %d, not PASS\r\n", UserDataCheckSum);
        return false;
    }
    
    /*检查注册个数*/
    if(head.DataCntSum != DataReg_Cnt)
    {
        DS_DEBUG_FMT("ERROR! DataCntSum = %d, not PASS\r\n", head.DataCntSum);
        return false;
    }
    
    /*移到用户数据区*/
    DataReg_OffsetAddr = sizeof(head);
    
    /*遍历注册列表*/
    for(uint32_t i = 0; i < head.DataCntSum; i++)
    {
        uint8_t high = EEPROM_ReadNext();
        uint8_t low = EEPROM_ReadNext();

        uint16_t UserSize = high << 8 | low;

        /*/校验是否与用户注册列表匹配*/
        if(UserSize != DataReg_List[i].Size)
        {
            DS_DEBUG_FMT("ERROR! UserSize = %d, not PASS\r\n", UserSize);
            return false;
        }

        /*将读取的数据写入注册列表中pData指针指向的数据*/
        for(uint32_t offset = 0; offset < UserSize; offset++)
            (DataReg_List[i].pData)[offset] = EEPROM_ReadNext();
    }
    
    /*读取成功*/
    DS_DEBUG("Read and check success");
    return true;
}
