#include "FileGroup.h"
#include "GUI_Private.h"

MillisTaskManager mtm_StatusBar(2);
ProgressBar PB_Batt(oled, 0);
uint8_t SignalStrength = 0;

float BattUsage = 100, BattVoltage = 4.2;

void Draw_BattUsage(int16_t x, int16_t y, int16_t w, int16_t h)
{
    PB_Batt.X = x;
    PB_Batt.Y = y;
    PB_Batt.Width = w;
    PB_Batt.Height = h;

    PB_Batt.setProgress(BattUsage / 100.0);
    oled.drawFastVLine(x + w, y + 2, h - 4, WHITE);
}

void Task_Update()
{
    SignalStrength = nrf.GetSignalStrength(); 
    oled.setTextSize(0);
    oled.setTextColor(WHITE, BLACK);
    oled.setCursor(2, 2);
    oled.printf("%3d%%", SignalStrength);
	
	oled.setCursor(35, 2);
    if(HMI_State)
    {
        oled.print("HMI");
    }
    else
    {
        oled.print("   ");
    }

    oled.setCursor(60, 2);
    if(RF_State)
    {
        oled.print("RF");
    }
    else
    {
        oled.print("  ");
    }
	
	oled.setCursor(80, 2);
    oled.setTextSize(0);
    oled.setTextColor(WHITE, BLACK);
    oled.printf("%1.1fV", BattVoltage);
    Draw_BattUsage(105, 1, 20, 10);
}

void Init_StatusBar()
{
    for(double i = 0.0; i < 1.0; i += 0.0005)
    {
        oled.drawFastHLine(0, StatusBar_POS, oled.width() / 2 * i, WHITE);
        oled.drawFastHLine(oled.width() - oled.width() / 2 * i - 1, StatusBar_POS, oled.width() / 2 * i, WHITE);
    }
    mtm_StatusBar.TaskRegister(0, Task_Update, 1000);
}
