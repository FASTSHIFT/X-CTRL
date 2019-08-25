#ifndef __TFT_S6B33B2_H
#define __TFT_S6B33B2_H

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "strprinter.h"

#define S6B33B2_TFTWIDTH_128  128
#define S6B33B2_TFTHEIGHT_160 160

#if defined(__STM32__) 
    typedef volatile uint32_t PortReg_t;
    #define USE_FAST_IO
#elif defined(__AVR__) 
    typedef volatile uint8_t PortReg_t;
    #define USE_FAST_IO
#endif

class TFT_S6B33B2 : public Adafruit_GFX{
public:
    TFT_S6B33B2(
        uint8_t port_start, uint8_t rst, uint8_t cs, 
        uint8_t rs, uint8_t rw, uint8_t rd
    );

    typedef enum{
        White = 0xFFFF,
        Black = 0x0000,
        Blue = 0x001F,
        Blue2 = 0x051F,
        Red = 0xF800,
        Magenta = 0xF81F,
        Green = 0x07E0,
        Cyan = 0x7FFF,
        Yellow = 0xFFE0
    }Color_Type;
    
    void begin();
    void writeCommond(uint8_t cmd);
    void writeData(uint16_t data);

#ifdef __STRPRINTER_H__    
		char *FontX;
    int printfX(const char *fmt, ...);
    int printfX(String str);
		void SetFontX(const char *font);
    void setOpacityX(uint8_t opacity);
#endif
    
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    virtual void setCursor(int16_t x, int16_t y);
    virtual void setRotation(uint8_t r);  
    virtual void fillScreen(uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    virtual void inline pushColor(uint16_t color)
    {
        writeData(color);
    }

private:
    PortReg_t* TFT_Port;
    uint8_t PORT_START; 
    uint8_t RST, CS, RS, RW, RD;
    uint8_t colstart, rowstart, xstart, ystart;

#if defined(USE_FAST_IO)
    #if defined(__AVR__) || defined(CORE_TEENSY)
        volatile uint8_t *rstport, *csport, *rsport, *rwport, *rdport;
        uint8_t  rstpinmask, cspinmask, rspinmask, rwpinmask, rdpinmask;
    #elif defined(__STM32__) 
        GPIO_TypeDef *rstport, *csport, *rsport, *rwport, *rdport;
        uint16_t  rstpinmask, cspinmask, rspinmask, rwpinmask, rdpinmask;
    #else
        volatile uint32_t *rstport, *csport, *rsport, *rwport, *rdport;
        uint32_t  rstpinmask, cspinmask, rspinmask, rwpinmask, rdpinmask;
    #endif
#endif

};

#endif
