#ifndef __GT911_DRIVER_H
#define __GT911_DRIVER_H

#include "Arduino.h"

#define GT9XX_IIC_RADDR 0xBB    //IIC read address
#define GT9XX_IIC_WADDR 0xBA    //IIC write address

#define GT9XX_IIC_ADDR 0x5D

#define GT9XX_READ_ADDR 0x814E  //touch point information
#define GT9XX_ID_ADDR 0x8140    //ID of touch IC

#define GT9XX_TOUCH_MAX 5

class GT911
{
    typedef struct{
        uint16_t x;
        uint16_t y;
    }Point_TypeDef;
public:
    GT911(int16_t int_pin, int16_t rst_pin = -1, int16_t w = 320, int16_t h = 480);
    bool init(uint8_t dir = 0);
    void intSync(uint16_t ms);
    void resetGuitar(uint8_t addr);
    void reset();
    void writeHandle (uint16_t addr);
    void writeData (uint16_t addr, uint8_t value);
    void readData (uint16_t addr, uint8_t cnt, uint8_t *value);
    uint8_t getCount(void);
    uint8_t scan(void);
    void sendConfig(void);

    Point_TypeDef point[GT9XX_TOUCH_MAX];

private:
    int16_t RST_Pin;
    int16_t INT_Pin;
    int16_t Width;
    int16_t Height;
    uint8_t Dir;
    uint8_t Status;
};



#endif
