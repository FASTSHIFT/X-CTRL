#include "FileGroup.h"

void HL_AT()
{
  Serial.println("AT");
  delay(100);
  Serial.println("AT+NAME=TREGGERS-CTRL");//命名模块名
  delay(100);
  Serial.println("AT+ROLE=0");//设置主从模式：0从机，1主机
  delay(100);
  Serial.println("AT+PSWD=4321");//设置配对密码，如1234
  delay(100);
  Serial.println("AT+UART=115200,0,0");//设置波特率115200，停止位1，校验位无
  tft.setCursor(0, 0);
  tft.print("ok");
}
void HL_print()
{
  char temp;
  temp = Serial.read();
  
  tft.setCursor(0, 0);
  tft.print(temp);
  //if(Press_BT ()==5)
  {
    tft.fillScreen(ST77XX_BLACK);
  }
}

