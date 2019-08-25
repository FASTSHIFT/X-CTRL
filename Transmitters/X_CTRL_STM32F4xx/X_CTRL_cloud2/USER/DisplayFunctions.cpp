
#include "filegroup.h"
#include "GUI_Private.h"

using namespace LightGUI;

void Page_ClearScreen(uint16_t color)
{
	screen.fillScreen(color);
}

void Page_DrawTaskBar()
{
	screen.fillRect(0, 0, screen.width(), TaskBarHeight, screen.Black);
	screen.setCursor(0, 0);
	screen.setTextColor(screen.White, screen.Black);
}

ProgressBar<SCREEN_CLASS> batt(&screen, 0, 4, 16, 16 * 0.618, 0);

void Page_DrawBattery()
{
	batt.X = screen.width() - 20;
	batt.Color_PB = screen.White;
	batt.Color_BG = screen.Black;
	batt.Color_FM = screen.White;
	batt.setProgress(BattUsage /  100.0);
	screen.drawFastVLine(batt.X + batt.Width, batt.Y + 2, batt.Height - 4, screen.White);
}

