#include "FileGroup.h"

uint8_t BT_print;

void bt_test()
{
//	tft.setTextSize(2);
//	tft.setTextWrap(true);
//    tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
//	tft.setCursor(0, 20);
//	tft.print(BT_print);
//	tft.print(BT_print);
//	tft.print(BT_print);
//	tft.setTextColor(ST77XX_BLACK);
//	tft.setCursor(0, 20);
//	tft.print(BT_print);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setCursor(0, 20);
    tft.print(BT_print, HEX);
    tft.print(" ");
}

void JS_Test() {
//		tft.drawRect(3,68,58,58,ST77XX_YELLOW);
//	tft.drawRect(100,68,58,58,ST77XX_YELLOW);
//  AX = map(JS_L.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 3,62);
//  AY = map(JS_L.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 68, 125);
//  tft.fillCircle(AX, AY, AR, ST77XX_BLUE);
//  if (oldAX != AX || oldAY != AY || oldAR != AR) {
//    tft.fillCircle(oldAX, oldAY, oldAR, ST77XX_BLACK);
//    tft.fillCircle(AX, AY, AR, ST77XX_BLUE);
//    oldAX = AX;
//    oldAY = AY;
//    oldAR = AR;
//  }


//  BX = map(JS_R.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 100,158);
//  BY = map(JS_R.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue,  -125,-68)*(-1);
//  tft.fillCircle(BX, BY, BR, ST77XX_RED);
//  if (oldBX != BX || oldBY != BY || oldBR != BR) {
//    tft.fillCircle(oldBX, oldBY, oldBR, ST77XX_BLACK);
//    tft.fillCircle(BX, BY, BR, ST77XX_RED);
//    oldBX = BX;
//    oldBY = BY;
//    oldBR = BR;
//  }
//  //  if (BX <= (AX + (AR / 2) + (BR / 2)) && BY <= (AY + (AR / 2) + (BR / 2)) && BX >= (AX - (AR / 2) - (BR / 2)) && BY >= (AY - (AR / 2) - (BR / 2)))
//  //  {
//  //    flag++;
//  //  }
//  //  if (flag == 20)
//  //  {
//  //    AR--;
//  //    flag=0;
//  //  }

//  //  RR = map(analogRead(R_R), 0, 4096, 0, tft.height());
//  //  LR = map(analogRead(L_R), 0, 4096, 0, tft.height());
//  //  tft.fillCircle(BX, BY, BR, ST77XX_RED);
//  //  tft.fillCircle(BX, BY, BR, ST77XX_RED);

bt_test();
}

