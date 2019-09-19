#include "TFT_ILI9488.h"

#define TFT_CMD_SET_X 0x2A
#define TFT_CMD_SET_Y 0x2B
#define TFT_CMD_WRITE_RAM 0x2C
#define TFT_CMD_READ_RAM 0x2D

#if defined(__STM32__)
#define TFT_RST_SET     GPIO_HIGH(rstport,rstpinmask)
#define TFT_CS_SET      GPIO_HIGH(csport,cspinmask)
#define TFT_RS_SET      GPIO_HIGH(rsport,rspinmask)
#define TFT_RW_SET      GPIO_HIGH(rwport,rwpinmask)
#define TFT_RD_SET      GPIO_HIGH(rdport,rdpinmask)

#define TFT_RST_CLR     GPIO_LOW(rstport,rstpinmask)
#define TFT_CS_CLR      GPIO_LOW(csport,cspinmask)
#define TFT_RS_CLR      GPIO_LOW(rsport,rspinmask)
#define TFT_RW_CLR      GPIO_LOW(rwport,rwpinmask)
#define TFT_RD_CLR      GPIO_LOW(rdport,rdpinmask)
#else
#define TFT_RST_SET     digitalWrite(RST,HIGH)
#define TFT_CS_SET      digitalWrite(CS,HIGH)
#define TFT_RS_SET      digitalWrite(RS,HIGH)
#define TFT_RW_SET      digitalWrite(RW,HIGH)
#define TFT_RD_SET      digitalWrite(RD,HIGH)

#define TFT_RST_CLR     digitalWrite(RST,LOW)
#define TFT_CS_CLR      digitalWrite(CS,LOW)
#define TFT_RS_CLR      digitalWrite(RS,LOW)
#define TFT_RW_CLR      digitalWrite(RW,LOW)
#define TFT_RD_CLR      digitalWrite(RD,LOW)
#endif

TFT_ILI9488::TFT_ILI9488(uint8_t port_start, uint8_t rst, uint8_t cs, uint8_t rs, uint8_t rw, uint8_t rd)
    : Adafruit_GFX(ILI9488_TFTWIDTH, ILI9488_TFTHEIGHT)
{
    PORT_START = port_start;
    TFT_Port = portOutputRegister(digitalPinToPort(PORT_START));
    RST = rst;
    CS = cs;
    RS = rs;
    RW = rw;
    RD = rd;

#if defined(__STM32__)
    rstport = digitalPinToPort(RST);
    rstpinmask = digitalPinToBitMask(RST);
    csport = digitalPinToPort(CS);
    cspinmask = digitalPinToBitMask(CS);
    rsport = digitalPinToPort(RS);
    rspinmask = digitalPinToBitMask(RS);
    rwport = digitalPinToPort(RW);
    rwpinmask = digitalPinToBitMask(RW);
    rdport = digitalPinToPort(RD);
    rdpinmask = digitalPinToBitMask(RD);
#endif
}

void TFT_ILI9488::begin()
{
    for(uint16_t pin = PORT_START; pin < PORT_START + 16; pin++)
    {
        pinMode(pin, OUTPUT);
    }

    pinMode(RST, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(RW, OUTPUT);
    pinMode(RD, OUTPUT);

    TFT_RD_SET;

    TFT_RST_CLR;
    delay(100);
    TFT_RST_SET;
    delay(50);
    
    //************* ILI9488³õÊ¼»¯**********//
    writeCommond(0XF7);
    writeData(0xA9);
    writeData(0x51);
    writeData(0x2C);
    writeData(0x82);

    writeCommond(0xC0);
    writeData(0x11);
    writeData(0x09);

    writeCommond(0xC1);
    writeData(0x41);

    writeCommond(0XC5);
    writeData(0x00);
    writeData(0x0A);
    writeData(0x80);

    writeCommond(0xB1);
    writeData(0xB0);
    writeData(0x11);

    writeCommond(0xB4);
    writeData(0x02);

    writeCommond(0xB6);
    writeData(0x02);
    writeData(0x22);

    writeCommond(0xB7);
    writeData(0xc6);

    writeCommond(0xBE);
    writeData(0x00);
    writeData(0x04);

    writeCommond(0xE9);
    writeData(0x00);

    writeCommond(0x36);
    writeData(0x08);

    writeCommond(0x3A);
    writeData(0x55);

    writeCommond(0xE0);
    writeData(0x00);
    writeData(0x07);
    writeData(0x10);
    writeData(0x09);
    writeData(0x17);
    writeData(0x0B);
    writeData(0x41);
    writeData(0x89);
    writeData(0x4B);
    writeData(0x0A);
    writeData(0x0C);
    writeData(0x0E);
    writeData(0x18);
    writeData(0x1B);
    writeData(0x0F);

    writeCommond(0XE1);
    writeData(0x00);
    writeData(0x17);
    writeData(0x1A);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x06);
    writeData(0x2F);
    writeData(0x45);
    writeData(0x43);
    writeData(0x02);
    writeData(0x0A);
    writeData(0x09);
    writeData(0x32);
    writeData(0x36);
    writeData(0x0F);

    writeCommond(0x11);
    delay(120);
    writeCommond(0x29);
    
    setRotation(0);
}

void TFT_ILI9488::writeCommond(uint8_t cmd)
{
    TFT_CS_CLR;
    TFT_RS_CLR;
    *TFT_Port = cmd;
    TFT_RW_CLR;
    TFT_RW_SET;
    TFT_CS_SET;
}

void TFT_ILI9488::writeData(uint16_t data)
{
    TFT_CS_CLR;
    TFT_RS_SET;
    *TFT_Port = data;
    TFT_RW_CLR;
    TFT_RW_SET;
    TFT_CS_SET;
}

#ifdef __STRPRINTER_H__
extern "C" {
#include <stdio.h>
#include <stdarg.h>
}
void TFT_ILI9488::setOpacityX(uint8_t opacity)
{
    SP_Brush_Opacity = opacity;
}
int TFT_ILI9488::printfX(const char *fmt, ...)
{
    static char TEXT_BUFFER[128];
    va_list  va;
    va_start(va, fmt);
    int ret = vsprintf(TEXT_BUFFER, fmt, va);
    va_end(va);

    SP_PrepareRect(Fonts_MicrosoftYahei11px, (char *)TEXT_BUFFER, 0);
    SP_FillBuffer_SolidBrush(textbgcolor);
    SP_CoverString(Fonts_MicrosoftYahei11px, (char *)TEXT_BUFFER, 0, textcolor);

    drawRGBBitmap(cursor_x, cursor_y, SP_Buffer, SP_Buffer_Width, SP_Buffer_Height);
    cursor_x += SP_Buffer_Width;

    return ret;
}

int TFT_ILI9488::printfX(String str)
{
    return printfX((const char*)str.c_str());
}
#endif

void TFT_ILI9488::setRotation(uint8_t r)
{
    rotation = r % 4;
    switch(rotation)
    {
        case 0:
        _width = ILI9488_TFTWIDTH;
        _height = ILI9488_TFTHEIGHT;
        writeCommond(0x36);
        writeData(1 << 3);
        break;
    case 1:
        _width = ILI9488_TFTHEIGHT;
        _height = ILI9488_TFTWIDTH;
        writeCommond(0x36);
        writeData((1 << 3) | (1 << 6) | (1 << 5));
        break;
    case 2:
        _width = ILI9488_TFTWIDTH;
        _height = ILI9488_TFTHEIGHT;
        writeCommond(0x36);
        writeData((1 << 3) | (1 << 6) | (1 << 7));
        break;
    case 3:
        _width = ILI9488_TFTHEIGHT;
        _height = ILI9488_TFTWIDTH;
        writeCommond(0x36);
        writeData((1 << 3) | (1 << 5) | (1 << 7));
        break;
    default:
        break;
    }
}

void TFT_ILI9488::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    if(x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0)
        return;
    
    writeCommond(TFT_CMD_SET_X);
    writeData(x0 >> 8);
    writeData(0x00FF & x0);
    writeData(x1 >> 8);
    writeData(0x00FF & x1);

    writeCommond(TFT_CMD_SET_Y);
    writeData(y0 >> 8);
    writeData(0x00FF & y0);
    writeData(y1 >> 8);
    writeData(0x00FF & y1);
    
    writeCommond(TFT_CMD_WRITE_RAM);
}

void TFT_ILI9488::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x + 1, y + 1);
}

void TFT_ILI9488::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x + 1, y + 1);
    writeData(color);
}

void TFT_ILI9488::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || h < 1) return;
    if((y + h - 1) >= _height) h = _height - y;
    if(x < 0) x = 0;
    if(y < 0) y = 0;

    setAddrWindow(x, y, x, y + h - 1);

    while (h--)
        writeData(color);
}

void TFT_ILI9488::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || w < 1) return;
    if((x + w - 1) >= _width)  w = _width - x;
    if(x < 0) x = 0;
    if(y < 0) y = 0;

    setAddrWindow(x, y, x + w - 1, y);

    while (w--)
        writeData(color);
}

void TFT_ILI9488::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    if((x >= _width) || (y >= _height)) return;

    int16_t actual_cursor_x = x;
    int16_t actual_cursor_y = y;
    int16_t actual_cursor_x1 = x + w - 1;
    int16_t actual_cursor_y1 = y + h - 1;

    if(actual_cursor_x < 0)
    {
        actual_cursor_x = 0;
    }
    else if(actual_cursor_x >= _width)
    {
        actual_cursor_x = _width - 1;
    }

    if(actual_cursor_y < 0)
    {
        actual_cursor_y = 0;
    }
    else if(actual_cursor_y >= _height)
    {
        actual_cursor_y = _height - 1;
    }

    if(actual_cursor_x1 < 0)
    {
        actual_cursor_x1 = 0;
    }
    else if(actual_cursor_x1 >= _width)
    {
        actual_cursor_x1 = _width - 1;
    }

    if(actual_cursor_y1 < 0)
    {
        actual_cursor_y1 = 0;
    }
    else if(actual_cursor_y1 >= _height)
    {
        actual_cursor_y1 = _height - 1;
    }

    setAddrWindow(actual_cursor_x, actual_cursor_y, actual_cursor_x1, actual_cursor_y1);

    for(int16_t Y = 0; Y < h; Y++)
    {
        for(int16_t X = 0; X < w; X++)
        {
            int16_t index = X + Y * w;
            int16_t actualX = x + X;
            int16_t actualY = y + Y;
            if(actualX >= 0 && actualX < _width && actualY >= 0 && actualY < _height)
            {
                writeData(bitmap[index]);
            }
        }
    }
}

void TFT_ILI9488::fastDrawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    uint32_t size = w * h;
    TFT_CS_CLR;
    TFT_RS_SET;
    for(uint32_t i = 0; i < size; i++)
    {
        *TFT_Port = bitmap[i];
        TFT_RW_CLR;
        TFT_RW_SET;
    }
    TFT_CS_SET;
}

void TFT_ILI9488::fillScreen(uint16_t color)
{
    setAddrWindow(0, 0, _width - 1, _height - 1);
    for(uint32_t i = 0; i < (_width * _height); i++)
        writeData(color);
}
