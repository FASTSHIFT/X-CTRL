#include "FileGroup.h"
int key_temp = 1;
int arrow = 0;
int arrow_last;
void page_up()
{
    arrow_last=arrow;
	arrow--;
	if(arrow<0){
		arrow=3;
	}
}
void page_down()
{
    arrow_last=arrow;
	arrow++;
	if(arrow>=4){
		arrow=0;
	}
}
void menu_page()
{
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(0, 0);
    tft.print("CTRL");
    tft.setCursor(0, 20);
    tft.print("bluetooth");
    tft.setCursor(0, 40);
    tft.print("NRF");
    tft.setCursor(0, 60);
    tft.print("JoyStick");


  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(60, arrow*20);
  tft.print("---");
  tft.setTextColor(ST77XX_BLACK);
  tft.setCursor(60, arrow_last*20);
  tft.print("---");

}
