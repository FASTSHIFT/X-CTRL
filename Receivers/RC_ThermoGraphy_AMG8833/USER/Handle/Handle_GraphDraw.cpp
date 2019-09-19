#include "FileGroup.h"

#define GRAPH_START_X 40
#define GRAPH_END_X 120

Adafruit_AMG88xx amg;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

uint8_t MaxTemp = MAXTEMP_DEFAULT;
uint8_t MinTemp = MINTEMP_DEFAULT;
uint8_t ResolutionRatio[6] = {5, 8, 10, 16, 20, 40};
uint8_t ResolutionRatio_Select = ResolutionRatio_SelectDefault;

//the colors we will be using
const uint16_t camColors[] = {0x480F,
                              0x400F, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810, 0x3010, 0x3010,
                              0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010, 0x2010, 0x1810, 0x1810,
                              0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811, 0x0811, 0x0811, 0x0011, 0x0011,
                              0x0011, 0x0011, 0x0011, 0x0031, 0x0031, 0x0051, 0x0072, 0x0072, 0x0092, 0x00B2,
                              0x00B2, 0x00D2, 0x00F2, 0x00F2, 0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192,
                              0x0192, 0x01B2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273,
                              0x0293, 0x02B3, 0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373,
                              0x0394, 0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
                              0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554, 0x0574,
                              0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571, 0x0591, 0x0591,
                              0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE, 0x05AE, 0x05AD, 0x05AD,
                              0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB, 0x05CA, 0x05CA, 0x05CA, 0x05C9,
                              0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7, 0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5,
                              0x05E5, 0x0604, 0x0604, 0x0604, 0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621,
                              0x0621, 0x0620, 0x0620, 0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640,
                              0x1E40, 0x1E40, 0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60,
                              0x3E60, 0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
                              0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0, 0x8EA0,
                              0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0, 0xB6E0, 0xB6E0,
                              0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0, 0xD700, 0xDF00, 0xDEE0,
                              0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640, 0xE620, 0xE600, 0xE5E0, 0xE5C0,
                              0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520, 0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480,
                              0xE460, 0xEC40, 0xEC20, 0xEC00, 0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40,
                              0xEB20, 0xEB00, 0xEAE0, 0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200,
                              0xF1E0, 0xF1C0, 0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0,
                              0xF080, 0xF060, 0xF040, 0xF020, 0xF800,
                             };


float AMG_OriginPixels[AMG_COLS * AMG_ROWS];

typedef struct {
    float temp;
    int16_t x;
    int16_t y;
} Temp_TypeDef;

void AMG_Drawpixels(float *p, uint8_t rows, uint8_t cols, uint8_t boxWidth, uint8_t boxHeight, bool showVal)
{
    uint8_t colorTemp;
    uint8_t NowMaxTemp = MaxTemp;
    uint8_t NowMinTemp = MinTemp;
    
    Temp_TypeDef Max, Min;
    Max.temp = NowMinTemp;
    Min.temp = NowMaxTemp;
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            float val = get_point(p, rows, cols, x, y);
            colorTemp = val = constrain(val, NowMinTemp, NowMaxTemp);

            if(val > Max.temp)
            {
                Max.temp = val;
                Max.x = x;
                Max.y = y;
            }
            else if(val < Min.temp)
            {
                Min.temp = val;
                Min.x = x;
                Min.y = y;
            }


            uint8_t colorIndex = map(colorTemp, NowMinTemp, NowMaxTemp, 0, 255);
//            colorIndex = constrain(colorIndex, 0, 255);

            tft.fillRect(GRAPH_START_X + boxWidth * x, boxHeight * y, boxWidth, boxHeight, camColors[colorIndex]);
        }
    }

    if (showVal)
    {
        tft.fillRect(GRAPH_END_X, 0, tft.width() - GRAPH_END_X, tft.height(), ST7735_BLACK);

        tft.setTextColor(ST7735_RED);

        tft.setTextSize(1);
        tft.setFont(&FreeSans9pt7b);
        tft.setCursor(128, 20);
        tft.print(MaxTemp);

        tft.setFont();
        tft.setCursor(GRAPH_START_X + boxWidth * Max.x, boxHeight * Max.y);
        tft.print(Max.temp, 1);


        tft.setTextColor(ST7735_BLUE);

        tft.setTextSize(1);
        tft.setFont(&FreeSans9pt7b);
        tft.setCursor(128, 70);
        tft.print(MinTemp);

        tft.setFont();
        tft.setCursor(GRAPH_START_X + boxWidth * Min.x, boxHeight * Min.y);
        tft.print(Min.temp, 1);

        tft.setCursor(2, 2);
        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
        tft.printf("@%d ", ResolutionRatio[ResolutionRatio_Select]);
    }
}
