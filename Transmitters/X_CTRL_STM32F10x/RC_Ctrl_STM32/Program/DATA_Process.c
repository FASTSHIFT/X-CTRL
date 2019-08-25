#include "FileGroup.h"

extern int L_PWM, L_X, L_Y;
extern int R_PWM, R_X, R_Y;
extern int B_PWM;
extern int posx,posy;
extern int L_CRE, R_CRE;
//extern const u8 Data_Head[2];
//extern const u8 Data_Tail[2];
extern u8 TX_buff[14];
extern u8 RX_buff[5];
extern u8 CTRL_object;

void Exchg()
{
  int lp, rp,bp;
  int lx, ly, rx, ry;
  switch (CTRL_object)
  {
    case CAR:
      lp = L_PWM + 300;
      TX_buff[0] = lp;
      TX_buff[1] = lp >> 8;

      rp = R_PWM + 300;
      TX_buff[2] = rp;
      TX_buff[3] = rp >> 8;

      TX_buff[4] = posx;
      TX_buff[5] = posx >> 8;

      TX_buff[6] = posy;
      TX_buff[7] = posy >> 8;

      TX_buff[13] = CAR;
      break;

    case TANK:
      lp = L_PWM + 300;
      TX_buff[0] = lp;
      TX_buff[1] = lp >> 8;

      rp = R_PWM + 300;
      TX_buff[2] = rp;
      TX_buff[3] = rp >> 8;

      bp = B_PWM + 300;
      TX_buff[4] = bp;
      TX_buff[5] = bp >> 8;

      TX_buff[13] = TANK;
      break;
  }
  if(CTRL_object==CPTR || CTRL_object==SPIDER)
  {
    lx = L_X + 300;
    TX_buff[0] = lx;
    TX_buff[1] = lx >> 8;
  
    ly = L_Y + 300;
    TX_buff[2] = ly;
    TX_buff[3] = ly >> 8;
  
    rx = R_X + 300;
    TX_buff[4] = rx;
    TX_buff[5] = rx >> 8;
  
    ry = R_Y + 300;
    TX_buff[6] = ry;
    TX_buff[7] = ry >> 8;
  
    TX_buff[8] = L_CRE;
    TX_buff[9] = L_CRE >> 8;
  
    TX_buff[10] = R_CRE;
    TX_buff[11] = R_CRE >> 8;
  
    //TX_buff[12] = (k_up << 7) | (k_down << 6) | (k_ok << 5) | (k_back << 4) | (k_l << 3) | (k_r << 2) | (k_jsl << 1) | k_jsr;
    TX_buff[13] = CTRL_object;
  }
}

//void SEND()
//{
//  if (CTRL_module == nRF)
//  {
//    /*nrf.Recv(RX_buff);
//    CPU_Temp = ( RX_buff[2] << 8 | RX_buff[1] ) / 10.0;
//    BATT_V = ( RX_buff[4] << 8 | RX_buff[3] ) / 10.0;
//    /*if (nrf.Tran_Recv(TX_buff,RX_buff) == 2)
//      {
//      CPU_Temp = ( RX_buff[2] << 8 | RX_buff[1] ) / 10.0;
//      BATT_V = ( RX_buff[4] << 8 | RX_buff[3] ) / 10.0;
//      recv_time = millis() + 2000;
//      }*/
//    nrf.Tran(TX_buff);
//  }
//  else
//  {
//    Serial.write(Data_Head, 2);
//    Serial.write(TX_buff, 14);
//    Serial.write(Data_Tail, 2);
//  }
//}

//void DATA_RECV(byte *rxbuff, byte _length)
//{
//  static byte rec_data;
//  static unsigned char dbs = 0;
//  if (Serial.available() > 0 && Serial.available() <= 60)
//  {
//    rec_data = Serial.read();
//    if (rec_data == 0xBB)dbs = 1, recv_time = millis() + 2000;
//    else
//    {
//      rxbuff[dbs] = rec_data;
//      dbs++;
//    }
//  }
//  else if (Serial.available() > 60)
//  {
//    lcd.clear();
//    lcd.print(" CACHE OVERFLOW!");
//    while (Serial.read() >= 0) {}; //清空串口缓存
//    lcd.clear();
//  }

//  if (dbs == _length)
//  {
//    CPU_Temp = ( RX_buff[2] << 8 | RX_buff[1] ) / 10.0;
//    BATT_V = ( RX_buff[4] << 8 | RX_buff[3] ) / 10.0;
//  }
//}
