#include "FileGroup.h"

//#include <EEPROM_SaveFloat.h>
//EEPROM_SaveFloat esf;

void DataStorageHandle(uint8_t Select)
{
    if (Select == ReadData)
    {
//    JS_L.Xmax = esf.read(0);
//    JS_L.Xmin = esf.read(1);
//    JS_L.Xmp = esf.read(2);
//    JS_L.Ymax = esf.read(3);
//    JS_L.Ymin = esf.read(4);
//    JS_L.Ymp = esf.read(5);

//    JS_R.Xmax = esf.read(6);
//    JS_R.Xmin = esf.read(7);
//    JS_R.Xmp = esf.read(8);
//    JS_R.Ymax = esf.read(9);
//    JS_R.Ymin = esf.read(10);
//    JS_R.Ymp = esf.read(11);

//    SendFreq = esf.read(12);
//    CtrlMode = esf.read(13);
//    CTRL_Module = esf.read(14);

//    ServoPos.X = ServoPos.Xmp = esf.read(15);
//    ServoPos.Y = ServoPos.Ymp = esf.read(16);

//    CTRL_Object = esf.read(17);
//    UART_Baud_Select = esf.read(18);
    }
    else if (Select == SaveData)
    {
//    esf.update(JS_L.Xmax, 0);
//    esf.update(JS_L.Xmin, 1);
//    esf.update(JS_L.Xmp, 2);
//    esf.update(JS_L.Ymax, 3);
//    esf.update(JS_L.Ymin, 4);
//    esf.update(JS_L.Ymp, 5);

//    esf.update(JS_R.Xmax, 6);
//    esf.update(JS_R.Xmin, 7);
//    esf.update(JS_R.Xmp, 8);
//    esf.update(JS_R.Ymax, 9);
//    esf.update(JS_R.Ymin, 10);
//    esf.update(JS_R.Ymp, 11);

//    esf.update(SendFreq, 12);
//    esf.update(CtrlMode, 13);
//    esf.update(CTRL_Module, 14);

//    esf.update(ServoPos.Xmp, 15);
//    esf.update(ServoPos.Ymp, 16);

//    esf.update(CTRL_Object, 17);
//    esf.update(UART_Baud_Select, 18);
    }
    else if (Select == ClearData) ;//esf.clear();
}

void ClearAllData()
{
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("Comfirm ?"));
    while (1)
    {
        Read_Keys();
        if (Key[k_ok])
        {
            Key[k_ok] = 0;
            lcd.clear();
            lcd.setCursor(2, 0);
            lcd.print(F("Waiting..."));
            DataStorageHandle(ClearData);
            lcd.clear();
            lcd.setCursor(2, 0);
            lcd.print(F("Clear Done!"));
            delay(1000);
            lcd.clear();
            break;
        }
        if (Key[k_back])
        {
            Key[k_back] = 0;
            lcd.clear();
            break;
        }
    }
}

//void EEPROM_Update(uint8_t* Data, uint32_t StartAddress, uint32_t DataLength)
//{
//  for (uint32_t i = 0; i < DataLength; i++)
//  {
//    EEPROM.update(Data[i], StartAddress + i);
//  }
//}

