#include "FileGroup.h"

uint8_t JS_LX()
{
  BX = map(analogRead(P_BX), 0, 4096, 0, 100);
  return BX;
}
uint8_t JS_LY()
{
  BY = map(analogRead(P_BY), 0, 4096, -100, 0) * (-1);
  return BY;
}
uint8_t JS_RX()
{
  AX = map(analogRead(P_AX), 0, 4096, 0, 100);
  return AX;
}
uint8_t JS_RY()
{
  AY = map(analogRead(P_AY), 0, 4096, 0, 100);
  return AY;
}
