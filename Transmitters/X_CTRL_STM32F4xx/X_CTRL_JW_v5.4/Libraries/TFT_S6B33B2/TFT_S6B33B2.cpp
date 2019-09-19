#include "TFT_S6B33B2.h"

/* LCD command set for Samsung S6B33B2 */
#define R_NOP                  0x00
#define R_OSCILLATION_MODE     0x02
#define R_DRIVER_OUTPUT_MODE   0x10
#define R_DCDC_SET             0x20
#define R_BIAS_SET             0x22
#define R_DCDC_CLOCK_DIV       0x24
#define R_DCDC_AMP_ONOFF       0x26
#define R_TEMP_COMPENSATION    0x28
#define R_CONTRAST_CONTROL1    0x2a
#define R_CONTRAST_CONTROL2    0x2b
#define R_STANDBY_OFF          0x2c
#define R_STANDBY_ON           0x2d
#define R_DDRAM_BURST_OFF      0x2e
#define R_DDRAM_BURST_ON       0x2f
#define R_ADDRESSING_MODE      0x30
#define R_ROW_VECTOR_MODE      0x32
#define R_N_LINE_INVERSION     0x34
#define R_FRAME_FREQ_CONTROL   0x36
#define R_RED_PALETTE          0x38
#define R_GREEN_PALETTE        0x3a
#define R_BLUE_PALETTE         0x3c
#define R_ENTRY_MODE           0x40
#define R_X_ADDR_AREA          0x42
#define R_Y_ADDR_AREA          0x43
#define R_RAM_SKIP_AREA        0x45
#define R_DISPLAY_OFF          0x50
#define R_DISPLAY_ON           0x51
#define R_SPEC_DISPLAY_PATTERN 0x53
#define R_PARTIAL_DISPLAY_MODE 0x55
#define R_PARTIAL_START_LINE   0x56
#define R_PARTIAL_END_LINE     0x57
#define R_AREA_SCROLL_MODE     0x59
#define R_SCROLL_START_LINE    0x5a
#define R_DATA_FORMAT_SELECT   0x60

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


TFT_S6B33B2::TFT_S6B33B2(uint8_t port_start, uint8_t rst, uint8_t cs, uint8_t rs, uint8_t rw, uint8_t rd)
    : Adafruit_GFX(S6B33B2_TFTWIDTH_128, S6B33B2_TFTHEIGHT_160)
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

void TFT_S6B33B2::begin()
{
    for(uint8_t pin = PORT_START; pin < PORT_START + 8; pin++)
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
    delay(250);
    TFT_RST_SET;
    delay(250);

    writeCommond(0x2C);
    delay(20);

    /*------set OTP mode off---------*/
    writeCommond(0xEA);
    delay(10);

    /*------set internal osc on---------*/
    writeCommond(0x02);
    writeCommond(0x01);
    delay(10);

    /*------booster1 on---------------*/
    writeCommond(0x20);
    writeCommond(0x0A);
    delay(10);

    /*------booster1 on---------------*/
    writeCommond(0x26);
    writeCommond(0x01);
    delay(10);

    /*------booster1 on and amp on---------*/
    writeCommond(0x26);
    writeCommond(0x09);
    delay(10);

    /*------booster2 on-------------*/
    writeCommond(0x26);
    writeCommond(0x0b);
    delay(10);

    /*------booster3 on-------------*/
    writeCommond(0x26);
    writeCommond(0x0f);
    delay(10);

    /*------temp compsation ratio -0.05%------*/
    writeCommond(0x28);
    writeCommond(0x01);

    /*------ram skip area set no skip------*/
    writeCommond(0x45);
    writeCommond(0x00);

    /*------normal display---------*/
    writeCommond(0x53);
    writeCommond(0x00);

    /*------set sdir=0,duty=1/162 swap=0-----*/
    writeCommond(0x10);
    writeCommond(34);   //34 or 39

    /*------set clock fpck=fose/32(Normal)/fpck=fose/16(partial1)-------*/
    writeCommond(0x24);
    writeCommond(0x11);

    /*------GSM=00:65K color,DSG=0,SGF=0,SGP=01,SGM=0-----*/
    writeCommond(0x30);
    writeCommond(0x02);//09

    /*------row vector type=Diagonal ,INC=111-----*/
    writeCommond(0x32);
    writeCommond(0x0E);//0e

    /*------entry mode set : x addr increce, read modify write off--*/
    writeCommond(0x40);
    writeCommond(0x08);    //0x82横屏

    /*------y address set from 00 to 127--------*/
    writeCommond(0x43);
    writeCommond(0x00);
    writeCommond(0x7F);

    /*------x address set from 00 to 159--------*/
    writeCommond(0x42);
    writeCommond(0x00);
    writeCommond(0x9F);

    /*------frame set FIM=ON,FIP=1FRAME,N-BLOCK=9-----*/
    writeCommond(0x34);
    writeCommond(0x89);//cd

    /*------contrast1 set v1 to 3.757v  max=4v----0x2A,CTRL1*/
    writeCommond(0x2a);
    writeCommond(0xCF);     /*partial display mode 0*/ //0xbb

    /*------contrast2 set v1 to 3.757v  max=4v--------*/
    writeCommond(0x2b);
    writeCommond(0xC8);     /*partial display mode 1*/ //0x20
    delay(10);

    /*------bias set to 1/5 --------*/
    writeCommond(0x22);
    writeCommond(0x11);

    /*------partial display mode off-------*/
    writeCommond(0x55);
    writeCommond(0x00);

    /*------Low frequency set off-------*/
    writeCommond(0x36);
    writeCommond(0x00);

    /*-------Scrol lstart line Set-------*/
    writeCommond(0x5A);
    writeCommond(0x00);

    /*------display on set--------*/
    delay(20);
    writeCommond(0x51);
}

void TFT_S6B33B2::writeCommond(uint8_t cmd)
{
    TFT_CS_CLR;
    TFT_RS_CLR;
    *(uint8_t*)TFT_Port = lowByte(cmd);
    TFT_RW_CLR;
    TFT_RW_SET;
    TFT_CS_SET;
}

void TFT_S6B33B2::writeData(uint16_t data)
{
    TFT_CS_CLR;
    TFT_RS_SET;
    *(uint8_t*)TFT_Port = lowByte(data >> 8);
    TFT_RW_CLR;
    TFT_RW_SET;

    *(uint8_t*)TFT_Port = lowByte(data);
    TFT_RW_CLR;
    TFT_RW_SET;
    TFT_CS_SET;
}

#ifdef __STRPRINTER_H__
extern "C" {
#include <stdio.h>
#include <stdarg.h>
}
void TFT_S6B33B2::setOpacityX(uint8_t opacity)
{
    SP_Brush_Opacity = opacity;
}
int TFT_S6B33B2::printfX(const char *fmt, ...)
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

int TFT_S6B33B2::printfX(String str)
{
    char buffer[128];
    str.toCharArray(buffer, str.length() + 1);
    return printfX(buffer);
}
#endif

void TFT_S6B33B2::setRotation(uint8_t r)
{
    rotation = r;
    if(rotation)
    {

    }
    else
    {

    }
}

void TFT_S6B33B2::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
// x 起始位置是2
    writeCommond(R_Y_ADDR_AREA);
    writeCommond(x0);
    writeCommond(x1);

    writeCommond(R_X_ADDR_AREA);
    writeCommond(y0);
    writeCommond(y1);
}

void TFT_S6B33B2::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x + 1, y + 1);
}

void TFT_S6B33B2::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x + 1, y + 1);
    writeData(color);
}

void TFT_S6B33B2::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
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

void TFT_S6B33B2::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
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

void TFT_S6B33B2::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
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

void TFT_S6B33B2::fillScreen(uint16_t color)
{
    setAddrWindow(0, 0, _width - 1, _height - 1);
    for(uint32_t i = 0; i < (_width * _height); i++)
        writeData(color);
}
