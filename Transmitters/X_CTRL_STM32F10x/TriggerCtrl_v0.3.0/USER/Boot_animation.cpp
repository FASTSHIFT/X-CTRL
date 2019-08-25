#include "FileGroup.h"
void Boot_logo()
{
	tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(3);
    tft.setCursor(17, 30);
    tft.print("TRIGGER");
    tft.setCursor(45, 70);
    tft.print("CTRL");
}
void Boot_logo2()
{
	int TextX=17,TextY=50;
	float a=1.9;
	tft.setTextSize(3);
	tft.setCursor(TextX+(0*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("T");
	tft.setCursor(TextX+(10*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("R");
	tft.setCursor(TextX+(20*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("I");
	tft.setCursor(TextX+(30*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("G");
	tft.setCursor(TextX+(40*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("G");
	tft.setCursor(TextX+(50*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("E");
	tft.setCursor(TextX+(60*a), TextY);
	tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
	tft.print("R");
}
