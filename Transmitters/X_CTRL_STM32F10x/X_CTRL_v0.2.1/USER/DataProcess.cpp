#include "FileGroup.h"

NRF nrf(NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, NRF_CE_Pin, NRF_CSN_Pin);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

const uint32_t UART_Baud[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200}; //常用波特率

const uint8_t Data_Head[2] = {0xAB, 0xCD};
const uint8_t Data_Tail[2] = {0xDC, 0xBA};
const uint8_t RX_DataLength = 18;
//const uint8_t TX_DataLength = 18;
uint8_t TX_buff[14];
uint8_t RX_buff[14];
uint8_t RX_DataPackage_Num;

uint32_t Recv_TimePoint;
uint32_t Tran_TimePoint;

DataPackage_TypeDef TX_Data;

//******从机******//
float Slave_Temp, Slave_BattVol;
float CR_kpa, CR_alt, CR_roll, CR_pitch, CR_FPV_Vol;
float Car_Kph, Car_RPS;
//***************//

void TX_DataLoad()
{
  int lp, rp, bp;
  switch (CTRL.Object)
  {
    case CAR:
      lp = L_PWM + 300;
      TX_buff[0] = lp;
      TX_buff[1] = lp >> 8;

      rp = R_PWM + 300;
      TX_buff[2] = rp;
      TX_buff[3] = rp >> 8;

      TX_buff[4] = ServoPos.X;
			TX_buff[5] = ServoPos.X >> 8;

			TX_buff[6] = ServoPos.Y;
			TX_buff[7] = ServoPos.Y >> 8;
		
			TX_buff[8] = TX_buff[8] + 1;
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
      break;
  }
	
  if (IS_USE_ORIGIN(CTRL.Object))
  {
    TX_Data.JS_LX = JS_L.X + 300;
    TX_Data.JS_LY = JS_L.Y + 300;
    TX_Data.JS_RX = JS_R.X + 300;
    TX_Data.JS_RY = JS_R.Y + 300;
		TX_Data.Rotoary_L = L_CRE/4;
		TX_Data.Rotoary_R = R_CRE/4;
		TX_Data.KeyValue = CompressToByte(Key);
		TX_Data.NowCtrlObject = CTRL.Object;
  }
	
	TX_buff[12] = CompressToByte(Key);
	TX_buff[13] = CTRL.Object;
}

void SendData(void)
{
	if(CTRL.Pattern == CtrlPattern_NRF)
	{
		if(IS_USE_ORIGIN(CTRL.Object))
		{
			nrf.TranRecv((uint8_t*)&TX_Data, RX_buff);
			//nrf.Tran((uint8_t*)&TX_Data);
		}
		else nrf.TranRecv(TX_buff, RX_buff);
	}
	else if(CTRL.Pattern == CtrlPattern_UART)
	{
		BlueTooth.write(Data_Head, sizeof(Data_Head));
		
		if(IS_USE_ORIGIN(CTRL.Object))
		{
			BlueTooth.write((uint8_t*)&TX_Data, sizeof(TX_Data));
		}
		else BlueTooth.write(TX_buff, sizeof(TX_buff));
		
		BlueTooth.write(Data_Tail, sizeof(Data_Head));
	}
}

void RX_DataLoad()
{
  Recv_TimePoint = millis() + 500;
  uint8_t rx_data[RX_DataLength * 2], dbs = 0; //数据包缓冲的缓冲，数据位
  for (uint8_t i = 0; i < RX_DataLength * 2; i++)
  {
    rx_data[i] = BlueTooth.read();//只抽取只有数据包长度的字节数
  }
  BlueTooth.flush();

  //  for (uint8_t i = 0; i < DATA_LETH * 2; i++) //显示原始数据
  //  {
  //    BlueTooth.print(rx_data[i], HEX);
  //    BlueTooth.print(' ');
  //  }
  //  BlueTooth.println(' ');

  while (dbs < RX_DataLength + 2)
  {
    if (rx_data[ dbs                       ] == Data_Head[0] &&
        rx_data[(dbs + 1)                  ] == Data_Head[1] &&
        rx_data[(dbs + (RX_DataLength - 2))] == Data_Tail[0] &&
        rx_data[(dbs + (RX_DataLength - 1))] == Data_Tail[1])
    {
      for (uint8_t i = 0; i < (RX_DataLength - 4); i++)//抽取包头包尾中间夹着的数据，填入缓冲区
      {
        RX_buff[i] = rx_data[dbs + 2 + i];
      }
      RX_Transform();//进入转换
      break;
    }
    dbs++;
  }
}

void RX_Transform()
{
  Slave_Temp = (RX_buff[1] << 8 | RX_buff[0]) / 10.0;
  Slave_BattVol = (RX_buff[3] << 8 | RX_buff[2]) / 10.0;
  //  if (CTRL_Object == CRAWLER)
  //  {
  //    CR_kpa = (RX_buff[5] << 8 | RX_buff[4]) / 100.0;
  //    CR_alt = (RX_buff[7] << 8 | RX_buff[6]) / 10.0;
  //
  //    CR_roll = RX_buff[9];
  //    if (!RX_buff[8])CR_roll = -CR_roll;
  //
  //    CR_pitch = RX_buff[11];
  //    if ((!RX_buff[10]))CR_pitch = -CR_pitch;
  //
  //    CR_FPV_Vol = RX_buff[12] / 10.0;
  //  }
  if (CTRL.Object == CAR)
  {
    Car_Kph = (RX_buff[5] << 8 | RX_buff[4]) / 100.0;
    Car_RPS = (RX_buff[7] << 8 | RX_buff[6]) / 100.0;
    RX_DataPackage_Num = RX_buff[13];
  }
}

void NRF_CheckConnection()
{
  static uint8_t RX_DataPackage_Num_Last;
  if (RX_DataPackage_Num != RX_DataPackage_Num_Last)Recv_TimePoint = millis() + 2000;
  RX_DataPackage_Num_Last = RX_DataPackage_Num;
}

uint8_t Get_ConnectionState()
{
	return(millis() < Recv_TimePoint || (CTRL.Pattern == CtrlPattern_UART && digitalRead(BT_State_Pin)));
}
