#ifndef SSD1306_I2C_H
#define SSD1306_I2C_H

#include "Arduino.h"
#include "Wire.h"
#include "Print.h"
#include "fonts/font8x8.h"

#define SSD1306_Max_X                 127    //128 Pixels
#define SSD1306_Max_Y                 63     //64  Pixels

#define PAGE_MODE                     01
#define HORIZONTAL_MODE               02

//#define SSD1306_Address               0x3C
#define SSD1306_Command_Mode          0x80
#define SSD1306_Data_Mode             0x40
#define SSD1306_Display_Off_Cmd       0xAE
#define SSD1306_Display_On_Cmd        0xAF
#define SSD1306_Normal_Display_Cmd    0xA6
#define SSD1306_Inverse_Display_Cmd   0xA7
#define SSD1306_Activate_Scroll_Cmd   0x2F
#define SSD1306_Dectivate_Scroll_Cmd  0x2E
#define SSD1306_Set_Brightness_Cmd    0x81

#define Scroll_Left                   0x00
#define Scroll_Right                  0x01

#define Scroll_2Frames                0x7
#define Scroll_3Frames                0x4
#define Scroll_4Frames                0x5
#define Scroll_5Frames                0x0
#define Scroll_25Frames               0x6
#define Scroll_64Frames               0x1
#define Scroll_128Frames              0x2
#define Scroll_256Frames              0x3

#define MAX_bitmaparrayCache 20

class SSD1306_I2C : public Print
{
  public:

    char addressingMode;
    void init(uint8_t _Address = 0x3C);
		virtual size_t write(uint8_t);
		using Print::write;

    void setCacheMode(uint8_t chs);
    void CachePutChar(uint8_t C);
    void putCache();

    void setNormalDisplay();
    void setInverseDisplay();

    void sendCommand(uint8_t command);
    void sendData(uint8_t Data);

    void setPageMode();
    void setHorizontalMode();

    void setCursor(uint8_t Column, uint8_t Row);
    void clear();
    void setBrightness(uint8_t Brightness);
    void putChar(uint8_t c);
		
		void drawBitmap(uint8_t x, uint8_t y, uint8_t* bitmaparray, uint8_t length);
		void drawBitmap(uint8_t *bitmaparray);
		void drawFullScreen(const uint8_t *PictureArray,int bytes);

    void setHorizontalScrollProperties(
        bool direction,
        uint8_t startPage, 
        uint8_t endPage, 
        uint8_t scrollSpeed);
    void activateScroll();
    void deactivateScroll();

   private:
   	
   	uint8_t SSD1306_Address;
    uint8_t LastCache[16][8];
    uint8_t Cache[16][8];

    uint8_t UseCache;
    uint8_t Cache_X,Cache_Y;
		uint8_t Pos;
		uint8_t *bitmaparrayCache[MAX_bitmaparrayCache];
};

#endif
