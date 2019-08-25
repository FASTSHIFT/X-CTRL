#include "FileGroup.h"
#include "GUI_Private.h"

MillisTaskManager mtm_StatusBar(2);
ProgressBar PB_Batt(screen, 0);

void Draw_BattUsage(int16_t x, int16_t y, int16_t w, int16_t h)
{
    PB_Batt.X = x;
    PB_Batt.Y = y;
    PB_Batt.Width = w;
    PB_Batt.Height = h;

    if(BattUsage < 30)
    {
        PB_Batt.Color_PB = screen.Red;
    }
    else if(BattUsage >= 90)
    {
        PB_Batt.Color_PB = screen.Green;
    }
    else
    {
        PB_Batt.Color_PB = screen.White;
    }

    PB_Batt.setProgress(BattUsage / 100.0);
    screen.drawFastVLine(x + w, y + 2, h - 4, screen.White);
}

void Task_Update()
{
    NRF_SignalStrength = nrf.GetSignalStrength();  
    screen.setTextSize(0);
    if(NRF_SignalStrength >= 90)screen.setTextColor(screen.Green, screen.Black);
    else if(NRF_SignalStrength >= 60)screen.setTextColor(screen.White, screen.Black);
    else if(NRF_SignalStrength >= 20)screen.setTextColor(screen.Yellow, screen.Black);
    else if(NRF_SignalStrength < 20)screen.setTextColor(screen.Red, screen.Black);
	screen.setCursor(2, 2);
    screen.printf("%3d%%", NRF_SignalStrength);

    screen.setCursor(80, 2);
    screen.setTextSize(0);
    screen.setTextColor(screen.Blue, screen.Black);
    screen.printf("%1.1fV", BattVoltage);
    Draw_BattUsage(105, 1, 20, 10);
	
	if(hc05.GetState())
		screen.setTextColor(screen.Green, screen.Black);
	else
		screen.setTextColor(screen.Yellow, screen.Black);
	screen.setCursor(35, 2);
    if(HMI_State)
    {
        screen.print("HMI");
    }
    else
    {
        screen.print("   ");
    }

    if(RF_State)
    {
		if(!digitalRead(SPDT_Switch_Pin))
			screen.setTextColor(screen.Yellow, screen.Black);
        else
			screen.setTextColor(screen.Green, screen.Black);
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
    }
    screen.setCursor(60, 2);
    screen.print("RF");
}

void Init_StatusBar()
{
    for(float i = 0.0; i < 1.0; i += 0.001)
    {
        screen.drawFastHLine(0, StatusBar_POS, screen.width() / 2 * i, screen.White);
        screen.drawFastHLine(screen.width() - screen.width() / 2 * i - 1, StatusBar_POS, screen.width() / 2 * i, screen.White);
    }
    mtm_StatusBar.TaskRegister(0, Task_Update, 1000);
}
