extern "C"
{
#include "FileGroup.h"
}
#include "NRF.h"
#include "DS1302.h"

NRF nrf1(PB15, PB14, PB13, PG6, PG7);//MOSI,MISO,SCK,CE,CSN
NRF nrf2(PE15, PE12, PE14, PB10, PB11);

//***************************Values*******************************//
int L_PWM, L_X, L_Y, L_XMP, L_YMP, L_XMAX, L_XMIN, L_YMAX, L_YMIN;
int R_PWM, R_X, R_Y, R_XMP, R_YMP, R_XMAX, R_XMIN, R_YMAX, R_YMIN;
int B_PWM;
int posx, posx_mp, posy, posy_mp;
int L_CRE, R_CRE;

//const u8 Data_Head[2] = {0xAB, 0xCD};
//const u8 Data_Tail[2] = {0xDC, 0xBA};
u8 TX_buff[1];
u8 RX_buff[1];

u8 k_up = 0, k_down = 0, k_l = 0, k_ok = 0, k_r = 0;		

u8 CTRL_State = 0;

unsigned int S_freq;
unsigned int Baud;
u8 CTRL_module;// 0:UART 1:NRF24L0
u8 CTRL_object;// 0:CAR 1:TANK 2:COMPUTER 3:SPIDER
u8 ctrl_md;
u8 BL_ON = On;
u8 Menu_Sta = OPT;//
u8 Silent_Mode = Off;

u8 i;
byte TX_ADR[5]={1,1,1,1,0};
byte RX_ADR[5]={1,1,1,1,0};

#define CE_PIN PC12
#define IO_PIN PC11
#define SCLK_PIN PC10

DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);

void setup(void)
{
	Serial_begin(115200);
	rtc.write_protect(false);
  rtc.halt(false);
  //       2017, 11, 13, 22: 21 :10 Monday
  Time t(2017, 11, 13, 22, 21, 10, 1);
  rtc.time(t);
//	pinMode(DS1,OUTPUT);
//	pinMode(DS2,OUTPUT);
//	
//	nrf1.SetAdress(TX_ADR);
//	nrf1.init(TX_MODE, TX_buff, 1, 0);//MODE, TX_buff, TX_width, RX_width
//	
//	nrf2.SetAdress(RX_ADR);
//	nrf2.init(RX_MODE, NULL, 0, 1);
//	
//	printf("NRF1:%d\r\n",nrf1.Status());
//	printf("NRF2:%d\r\n",nrf2.Status());
//	delay(2000);
//	digitalWrite(DS2,HIGH);
}

void loop(void)
{
	Time t = rtc.time();
	printf("Year:%d , Mon:%d , Date:%d , Week:%d , %d:%d:%d",
					t.yr,			t.mon,		t.date,		t.day,	t.hr,t.min,t.sec);
	delay(1000);
//	i++;
//	TX_buff[0]=i;
//	nrf1.Tran(TX_buff);
//	printf("NRF1 tran:%d\t",i);
//	digitalWrite(DS1,LOW);
//	delay(100);
//	nrf2.Recv(RX_buff);
//	printf("NRF2 recv:%d\r\n\n",RX_buff[0]);
//	digitalWrite(DS1,HIGH);
//	delay(1000);
}

//*****************MAIN******************//
int main(void)
{
	delay_init();
	ADCx_Init();
	setup();
	while(1)loop();
}
