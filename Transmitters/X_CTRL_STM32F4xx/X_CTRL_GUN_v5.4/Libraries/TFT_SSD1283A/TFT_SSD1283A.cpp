#include "TFT_SSD1283A.h"
#include "SPI.h"

#define TFT_SPI SPI

#if defined(__STM32__)
#define	LCD_CS_SET  	GPIO_HIGH(csport,cspinmask)
#define	LCD_RST_SET  	GPIO_HIGH(resetport,resetpinmask)
#define	LCD_DC_SET  	GPIO_HIGH(dcport,dcpinmask)
#define	LCD_SDA_SET  	GPIO_HIGH(sdaport,sdapinmask)
#define	LCD_SCK_SET  	GPIO_HIGH(sckport,sckpinmask)

#define	LCD_CS_CLR  	GPIO_LOW(csport,cspinmask)
#define	LCD_RST_CLR  	GPIO_LOW(resetport,resetpinmask)
#define	LCD_DC_CLR  	GPIO_LOW(dcport,dcpinmask)
#define	LCD_SDA_CLR  	GPIO_LOW(sdaport,sdapinmask)
#define	LCD_SCK_CLR  	GPIO_LOW(sckport,sckpinmask)
#endif

TFT_SSD1283A::TFT_SSD1283A(uint8_t cs, uint8_t reset, uint8_t dc, uint8_t sda, uint8_t sck)
    : Adafruit_GFX(SSD1283A_TFTWIDTH_130, SSD1283A_TFTHEIGHT_130)
{
    hwspi = false;
    cs_pin = cs;
    reset_pin = reset;
    dc_pin = dc;
    sda_pin = sda;
    sck_pin = sck;

#if defined(__STM32__)
    csport = digitalPinToPort(cs_pin);
    cspinmask = digitalPinToBitMask(cs_pin);
    resetport = digitalPinToPort(reset_pin);
    resetpinmask = digitalPinToBitMask(reset_pin);
    dcport = digitalPinToPort(dc_pin);
    dcpinmask = digitalPinToBitMask(dc_pin);
    sdaport = digitalPinToPort(sda_pin);
    sdapinmask = digitalPinToBitMask(sda_pin);
    sckport = digitalPinToPort(sck_pin);
    sckpinmask = digitalPinToBitMask(sck_pin);
#endif
}

TFT_SSD1283A::TFT_SSD1283A(uint8_t cs, uint8_t reset, uint8_t dc)
    : Adafruit_GFX(SSD1283A_TFTWIDTH_130, SSD1283A_TFTHEIGHT_130)
{
    hwspi = true;
    cs_pin = cs;
    reset_pin = reset;
    dc_pin = dc;

#if defined(__STM32__)
    csport = digitalPinToPort(cs_pin);
    cspinmask = digitalPinToBitMask(cs_pin);
    resetport = digitalPinToPort(reset_pin);
    resetpinmask = digitalPinToBitMask(reset_pin);
    dcport = digitalPinToPort(dc_pin);
    dcpinmask = digitalPinToBitMask(dc_pin);
#endif
}

void TFT_SSD1283A::begin()
{
    pinMode(cs_pin, OUTPUT);
    pinMode(dc_pin, OUTPUT);
    pinMode(reset_pin, OUTPUT);

    if(!hwspi)
    {
        pinMode(sda_pin, OUTPUT);
        pinMode(sck_pin, OUTPUT);
    }
    else
    {
        TFT_SPI.begin();
        TFT_SPI.setClock(120000000);
    }

    reset(); //Reset before LCD Init.

    writeReg(0x10, 0x2F8E); /* power control 1 */
    writeReg(0x11, 0x000C); /* power control 2 */
    writeReg(0x07, 0x0021); /* display control */
    writeReg(0x28, 0x0006); /* vcom OTP */
    writeReg(0x28, 0x0005);
    writeReg(0x27, 0x057F); /* further bias current setting */
    writeReg(0x29, 0x89A1); /* vcom OTP */
    writeReg(0x00, 0x0001); /* OSC en */
    delay_ms(100);
    writeReg(0x29, 0x80B0); /* vcom OTP */
    delay_ms(30);
    writeReg(0x29, 0xFFFE); /* vcom OTP */
    writeReg(0x07, 0x0023); /* display control */
    delay_ms(30);
    writeReg(0x07, 0x0033); /* display control */

    writeReg(0x01, 0x2283); /* driver output control, REV, TB=0, RL=1, RGB */
    writeReg(0x03, 0x6838); /* entry mode, 65K, ram, ID3 */

    writeReg(0x2F, 0xFFFF); /* 2A ~ 2F, test */
    writeReg(0x2C, 0x8000);
    writeReg(0x27, 0x0570);
    writeReg(0x02, 0x0300); /* driving wave form control */
    writeReg(0x0B, 0x580C); /* frame cycle control */
    writeReg(0x12, 0x0609); /* power control 3 */
    writeReg(0x13, 0x3100); /* power control 4 */

    setRotation(1);
}

void TFT_SSD1283A::spiWrite(uint8_t data)
{
    if(hwspi)
    {
        TFT_SPI.transfer(data);
    }
    else
    {
        for(uint8_t i = 8; i > 0; i--)
        {
            (data & 0x80) ? LCD_SDA_SET : LCD_SDA_CLR;

            LCD_SCK_CLR;
            LCD_SCK_SET;
            data <<= 1;
        }
    }
}

extern "C"
{
#include <stdio.h>
#include <stdarg.h>
}
void TFT_SSD1283A::setOpacityX(uint8_t opacity)
{
    SP_Brush_Opacity = opacity;
}
int TFT_SSD1283A::printfX(const char *fmt, ...) {
    static char TEXT_BUFFER[128];
    va_list  va;
    va_start(va, fmt);
    int ret = vsprintf(TEXT_BUFFER, fmt, va);
    va_end(va);

    SP_PrepareRect(Fonts_MicrosoftYahei11px, (char *)TEXT_BUFFER, 0);
    SP_FillBuffer_SolidBrush(textbgcolor);
    SP_CoverString(Fonts_MicrosoftYahei11px, (char *)TEXT_BUFFER, 0, textcolor);

    /*int actual_cursor_x = cursor_x;
    int actual_cursor_y = cursor_y;
    int actual_cursor_x1 = cursor_x + SP_Buffer_Width - 1;
    int actual_cursor_y1 = cursor_y + SP_Buffer_Height - 1;

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

    for(int y = 0; y < SP_Buffer_Height; y++)
    {
    	for(int x = 0; x < SP_Buffer_Width; x++)
    	{
    		int index = x + y * SP_Buffer_Width;
    		int actualX = cursor_x + x;
    		int actualY = cursor_y + y;
    		if(actualX >=0 && actualX < _width && actualY >= 0 && actualY < _height)
    		{
    			pushColor(SP_Buffer[index]);
    		}
    	}
    }*/
    drawRGBBitmap(cursor_x, cursor_y, SP_Buffer, SP_Buffer_Width, SP_Buffer_Height);
    cursor_x += SP_Buffer_Width;

    return ret;
}

int TFT_SSD1283A::printfX(String str) {
    char buffer[128];
    str.toCharArray(buffer, str.length() + 1);
    return printfX(buffer);
}

void TFT_SSD1283A::writeCommond(uint8_t cmd)
{
    LCD_CS_CLR;
    LCD_DC_CLR;
    spiWrite(cmd);
    LCD_CS_SET;
}

void TFT_SSD1283A::writeData(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    spiWrite(data);
    LCD_CS_SET;
}

void TFT_SSD1283A::writeReg(uint8_t reg, uint16_t data)
{
    writeCommond(reg);
    writeData16(data);
}

void TFT_SSD1283A::writeData16(uint16_t data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    spiWrite(data >> 8);
    spiWrite(data);
    LCD_CS_SET;
}

void TFT_SSD1283A::reset()
{
    LCD_RST_CLR;
    delay(100);
    LCD_RST_SET;
    delay(50);
}

void TFT_SSD1283A::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setPoint(x, y);
}


void TFT_SSD1283A::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    if(rotation)
    {
        writeCommond(0x45);
        writeData(x1);
        writeData(x0);

        writeCommond(0x44);
        writeData(y1 + 2);
        writeData(y0 + 2);
    }
    else
    {
        writeCommond(0x44);
        writeData(x1 + 2);
        writeData(x0 + 2);

        writeCommond(0x45);
        writeData(y1 + 2);
        writeData(y0 + 2);
    }
    setPoint(x0, y0);
}

void TFT_SSD1283A::setRotation(uint8_t r)
{
    rotation = r;
    if(rotation)
    {
        writeReg(0x01, 0x2283); /* driver output control, REV, TB=0, RL=1, RGB */
        writeReg(0x03, 0x6838); /* entry mode, 65K, ram, ID3 */
    }
    else
    {
        writeReg(0x01, 0x2183); /* driver output control, REV, TB, RGB */
        writeReg(0x03, 0x6830); /* entry mode, 65K, ram, ID0 */
    }
}

void TFT_SSD1283A::setPoint(int16_t x, int16_t y)
{
    if(rotation)
    {
        writeCommond(0x21);
        writeData(x);
        writeData(y + 2);
    }
    else
    {
        writeCommond(0x21);
        writeData(y + 2);
        writeData(x + 2);
    }
    writeCommond(0x22);
}

void TFT_SSD1283A::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setPoint(x, y);
    writeData16(color);
}

void TFT_SSD1283A::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || h < 1) return;
    if((y + h - 1) >= _height) h = _height - y;
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    
    setAddrWindow(x, y, x, y + h - 1);

    while (h--)
        writeData16(color);
}


void TFT_SSD1283A::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || w < 1) return;
    if((x + w - 1) >= _width)  w = _width - x;
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    
    setAddrWindow(x, y, x + w - 1, y);

    while (w--)
        writeData16(color);
}

void TFT_SSD1283A::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
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
    		if(actualX >=0 && actualX < _width && actualY >= 0 && actualY < _height)
    		{
    			pushColor(bitmap[index]);
    		}
    	}
    }
}

void TFT_SSD1283A::pushColor(uint16_t color)
{
    writeData16(color);
}

void TFT_SSD1283A::fillScreen(uint16_t color)
{
    setAddrWindow(0, 0, _width - 1, _height - 1);
    for(uint32_t i = 0; i < (_width * _height); i++)
        writeData16(color);
}
