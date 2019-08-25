#include "NRF.h"

byte ADDRESS[ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};
u8 TX_PLOAD_WIDTH;
u8 RX_PLOAD_WIDTH;

void NRF_init(u8 mode, u8 *buff, u8 TX_length, u8 RX_length)
{
	pinMode(CE,OUTPUT);
  pinMode(CSN,OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(MISO,INPUT);
  pinMode(IRQ,INPUT);
  pinMode(SCK,OUTPUT);
	
  digitalWrite(CE,LOW);
  digitalWrite(CSN,HIGH);
	
	switch(mode)
	{
		case RX_MODE:
			RX_PLOAD_WIDTH=RX_length;
			RX_Mode();
			break;
		
		case TX_MODE:
			TX_PLOAD_WIDTH=TX_length;
			TX_Mode(buff);
			break;
		
		case TXRX_MODE:
			RX_PLOAD_WIDTH=RX_length;
			TX_PLOAD_WIDTH=TX_length;
			TX_Mode(buff);
			break;
	}
}

byte NRF_Status(void)
{
  if(SPI_Read(STATUS_)!=0)return 1;
  else return 0;
}


byte SPI_RW(u8 Byte)
{
	byte i;
  for(i=0;i<8;i++)
  {
    if(Byte&0x80)digitalWrite(MOSI,HIGH);
    else digitalWrite(MOSI,LOW);
    Byte<<=1;
    if(digitalRead(MISO)==1) Byte|=1;
  }
	return Byte;
	//return SPI_ReadWriteByte(Byte);
}

byte SPI_Read_Buf(byte reg,byte *pBuf,byte bytes)
{
  byte status_,i;

  digitalWrite(CSN,LOW);
  status_=SPI_RW(reg);
  for(i=0;i<bytes;i++) pBuf[i]=SPI_RW(0);
  digitalWrite(CSN,HIGH);
  return status_;
}

byte SPI_RW_Reg(byte reg,byte value)
{
  byte status_;

  digitalWrite(CSN,LOW);
  status_=SPI_RW(reg);
  SPI_RW(value);
  digitalWrite(CSN,HIGH);
  return status_;
}

byte SPI_Read(byte reg)
{
  byte reg_val;
  digitalWrite(CSN,LOW);
  SPI_RW(reg);
  reg_val=SPI_RW(0);
  digitalWrite(CSN,HIGH);
  return reg_val;
}

byte SPI_Write_Buf(byte reg,byte *pBuf,byte bytes)
{
  byte status_,i;

  digitalWrite(CSN,LOW);
  status_=SPI_RW(reg);
  for(i=0;i<bytes;i++) SPI_RW(*pBuf++);
  digitalWrite(CSN,HIGH);
  return status_;
}

void RX_Mode()
{
  digitalWrite(CE,LOW);
  SPI_Write_Buf(_WRITE_REG+RX_ADDR_P0,ADDRESS,ADR_WIDTH);
  SPI_RW_Reg(_WRITE_REG+EN_AA,0x01);
  SPI_RW_Reg(_WRITE_REG+EN_RXADDR,0x01);
  SPI_RW_Reg(_WRITE_REG+RF_CH,40);
  SPI_RW_Reg(_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
  SPI_RW_Reg(_WRITE_REG+RF_SETUP,0x07);
  SPI_RW_Reg(_WRITE_REG+CONFIG,0x0F);
  digitalWrite(CE,HIGH);
}

byte RX_DATA(byte *rxbuff)
{
  byte status_=SPI_Read(STATUS_);
  if(status_&RX_DR)
  {
    SPI_Read_Buf(RD_RX_PLOAD,rxbuff,RX_PLOAD_WIDTH);
    SPI_RW_Reg(FLUSH_RX,0);
  }
  SPI_RW_Reg(_WRITE_REG+STATUS_,status_);
	return status_;
}

void TX_Mode(byte *txbuff)
{
  digitalWrite(CE,LOW);
  SPI_Write_Buf(_WRITE_REG+TX_ADDR,ADDRESS,ADR_WIDTH);
  SPI_Write_Buf(_WRITE_REG+RX_ADDR_P0,ADDRESS,ADR_WIDTH);

  SPI_RW_Reg(_WRITE_REG+EN_AA,0x01);
  SPI_RW_Reg(_WRITE_REG+EN_RXADDR,0x01);
  SPI_RW_Reg(_WRITE_REG+CONFIG,0x0E);
  SPI_RW_Reg(_WRITE_REG+RF_CH,40);
  SPI_RW_Reg(_WRITE_REG+RF_SETUP,0x07);
  SPI_RW_Reg(_WRITE_REG+SETUP_RETR,0x1A);
  SPI_Write_Buf(WR_TX_PLOAD,txbuff,TX_PLOAD_WIDTH);

  digitalWrite(CE,HIGH);
}

byte TX_DATA(byte *txbuff)
{
  byte status_;
  status_=SPI_Read(STATUS_);
  if(status_&TX_DS)
  {
    SPI_RW_Reg(FLUSH_TX,0);
    SPI_Write_Buf(WR_TX_PLOAD,txbuff,TX_PLOAD_WIDTH);
  }
  if(status_&MAX_RT)
  {
    SPI_RW_Reg(FLUSH_TX,0);
    SPI_Write_Buf(WR_TX_PLOAD,txbuff,TX_PLOAD_WIDTH);
  }
  SPI_RW_Reg(_WRITE_REG+STATUS_,status_);
	return status_;
}

byte TXRX_DATA(byte *txbuff,byte *rxbuff)
{
	static byte Tx_Cnt=0,Rx_Cnt=0;
  static byte TXMode=0,ret=0;
  if(TXMode)
  {
    if(TX_DATA(txbuff)==TX_DS||TX_DATA(txbuff)==MAX_RT||Tx_Cnt==Recon_Delay_Cnt)
    {
      RX_Mode();
			ret=0;
      TXMode=0;
      Tx_Cnt=0;
    }
    Tx_Cnt++;
  }
  else
  {
    if(RX_DATA(rxbuff)==RX_DR||Rx_Cnt==Recon_Delay_Cnt)
    {
      TX_Mode(txbuff);
			ret=1;
      TXMode=1;
      Rx_Cnt=0;
    }
    Rx_Cnt++;
  }
	return ret;
}
