#ifndef __TFT_SSD1283A
#define __TFT_SSD1283A

#include "Adafruit_GFX.h"
#include "strprinter.h"

namespace TFT_SSD1283A_GPIO_TypeDef{
    #if defined(__AVR__)
        #include <avr/pgmspace.h>
        typedef volatile uint8_t PortReg_t;
        typedef uint8_t PortMask_t;
    #elif defined(__STM32__)
        typedef GPIO_TypeDef PortReg_t;
        typedef uint16_t PortMask_t;
    #endif
}

#define SSD1283A_RED  	0xf800    //红色
#define SSD1283A_GREEN	0x07e0    //绿色
#define SSD1283A_BLUE 	0x001f    //蓝色
#define SSD1283A_SBLUE 	0x251F    //淡蓝色
#define SSD1283A_WHITE	0xffff    //白色
#define SSD1283A_BLACK	0x0000    //黑色
#define SSD1283A_YELLOW 0xFFE0    //黄色
#define SSD1283A_GRAY0  0xEF7D    //灰色0 1110 1111 0111 1100 
#define SSD1283A_GRAY1  0x8410    //灰色1 1000 0100 0001 0000
#define SSD1283A_GRAY2  0x4208    //灰色2 0100 0010 0000 1000

#define SSD1283A_TFTWIDTH_130 130
#define SSD1283A_TFTHEIGHT_130 130

class TFT_SSD1283A : public Adafruit_GFX {
public:
    TFT_SSD1283A(uint8_t cs, uint8_t reset, uint8_t dc, uint8_t sda, uint8_t sck);
    TFT_SSD1283A(uint8_t cs, uint8_t reset, uint8_t dc);

    void begin();
    void reset();
    void writeCommond(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void writeReg(uint8_t reg, uint16_t data);
    void setPoint(int16_t x, int16_t y);
   
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    virtual void setCursor(int16_t x, int16_t y);
    virtual void setRotation(uint8_t r);
    virtual void pushColor(uint16_t color);
    virtual void fillScreen(uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    
    int printfX(const char *fmt, ...);
    int printfX(String str);
    void setOpacityX(uint8_t opacity);

    typedef enum {
        Black = SSD1283A_BLACK,
        Blue = SSD1283A_BLUE,
        Red = SSD1283A_RED,
        Green = SSD1283A_GREEN,
        Yellow = SSD1283A_YELLOW,
        White = SSD1283A_WHITE,
    } Color_Type;
    
private:
    bool hwspi;
    uint8_t cs_pin, reset_pin, dc_pin, sda_pin, sck_pin; 
    #if defined(__AVR__) || defined(__STM32__)  
        TFT_SSD1283A_GPIO_TypeDef::PortReg_t  *csport, *resetport, *dcport, *sdaport, *sckport;
        TFT_SSD1283A_GPIO_TypeDef::PortMask_t cspinmask, resetpinmask, dcpinmask, sdapinmask, sckpinmask;
    #endif
    void spiWrite(uint8_t data);
};

#endif
