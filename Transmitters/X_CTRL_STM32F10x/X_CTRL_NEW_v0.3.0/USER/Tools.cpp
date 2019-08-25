#include "FileGroup.h"

uint32_t MainLoop_Speed;

void IIC_Scan()
{
    uint8_t error, address;
    int nDevices;

//  Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
//     Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    //delay(2000); // wait 5 seconds for next scan
}

void Limit_int16(int16_t *data, int16_t MIN, int16_t MAX)
{
    if (*data < MIN)*data = MIN;
    else if (*data > MAX)*data = MAX;
}

void LoopExecution(void_func_point Func, uint32_t Times)
{
    for(uint32_t i = 0; i < Times; i++)
    {
        Func();
    }
}

void LoopSpeedDetect()
{
    static uint32_t TimePoint, LoopCnt;
    LoopCnt++;
    if(millis() > TimePoint)
    {
        MainLoop_Speed = LoopCnt;
        LoopCnt = 0;
        TimePoint = millis() + 1000;
    }
}
