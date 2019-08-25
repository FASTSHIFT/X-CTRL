#include "FileGroup.h"

extEEPROM eep(kbits_8, 1, 64, 0x50);//device size, number of devices, page size,I2C Address

union ExgStruct_CtrlParamter
{
    struct CtrlParamter StructCache;
    uint8_t data_u8[sizeof(CtrlParamter)];
} ESCP;

union ExgStruct_CtrlOption
{
    struct CtrlOption StructCache;
    uint8_t data_u8[sizeof(CtrlOption)];
} ESCO;

void DataRead_Struct_CtrlParamter(u16 addr, CtrlParamter &Struct)
{
    for (uint32_t i = 0; i < sizeof(CtrlParamter); i++)
    {
        ESCP.data_u8[i] = eep.read(addr + i);
    }
    Struct = ESCP.StructCache;
}

void DataUpdate_Struct_CtrlParamter(u16 addr, CtrlParamter &Struct)
{
    ESCP.StructCache = Struct;
    for (uint32_t i = 0; i < sizeof(CtrlParamter); i++)
    {
        eep.update(addr + i, ESCP.data_u8[i]);
    }
}

void DataRead_Struct_CtrlOption(u16 addr, CtrlOption &Struct)
{
    for (uint32_t i = 0; i < sizeof(CtrlOption); i++)
    {
        ESCO.data_u8[i] = eep.read(addr + i);
    }
    Struct = ESCO.StructCache;
}

void DataUpdate_Struct_CtrlOption(u16 addr, CtrlOption &Struct)
{
    ESCO.StructCache = Struct;
    for (uint32_t i = 0; i < sizeof(CtrlOption); i++)
    {
        eep.update(addr + i, ESCO.data_u8[i]);
    }
}

void DataClear(uint32_t EndAddr)
{
    eep.write(0, NULL, EndAddr);
}

void DataStorageHandle(DataStorageHandle_TypeDef Select)
{
    if (Select == ReadData)
    {
        DataRead_Struct_CtrlParamter(0, JS_L);
        DataRead_Struct_CtrlParamter(70, JS_R);
        DataRead_Struct_CtrlParamter(140, ServoPos);
        DataRead_Struct_CtrlOption(200, CTRL);
    }
    else if (Select == SaveData)
    {
        DataUpdate_Struct_CtrlParamter(0, JS_L);
        DataUpdate_Struct_CtrlParamter(70, JS_R);
        DataUpdate_Struct_CtrlParamter(140, ServoPos);
        DataUpdate_Struct_CtrlOption(200, CTRL);
    }
    else if (Select == ClearData)
    {
        DataClear(eep.length() / 8);
    }
}

void CTRL_SetDefault()
{
    while (1)
    {
        Read_Keys();
        if (Key[k_ok])
        {
            Key[k_ok] = 0;

            //DataStorageHandle(ClearData);
            CTRL.Baud_Select = UART_Baud_Select_Default;
            CTRL.SendFreq = SendFreq_Default;
            CTRL.Mode = CtrlMode_Default;
            CTRL.Pattern = CtrlPattern_UART;
            CTRL.Object = CtrlObject_Default;
            ServoPos.Xmp = 90;
            ServoPos.Ymp = 90;
            DataStorageHandle(SaveData);

            delay(1000);
            break;
        }
        if (Key[k_back])
        {
            Key[k_back] = 0;
            break;
        }
    }
}
