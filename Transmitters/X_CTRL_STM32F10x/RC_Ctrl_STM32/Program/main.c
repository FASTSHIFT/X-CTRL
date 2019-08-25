#include "FileGroup.h"
#include "NRF.h"

//***************************Values*******************************//
int L_PWM, L_X, L_Y, L_XMP, L_YMP, L_XMAX, L_XMIN, L_YMAX, L_YMIN;
int R_PWM, R_X, R_Y, R_XMP, R_YMP, R_XMAX, R_XMIN, R_YMAX, R_YMIN;
int B_PWM;
int posx, posx_mp, posy, posy_mp;
int L_CRE, R_CRE;

const u8 Data_Head[2] = {0xAB, 0xCD};
const u8 Data_Tail[2] = {0xDC, 0xBA};
u8 TX_buff[14];
u8 RX_buff[5];

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

int i;

void setup(void)
{
	Serial_begin(115200);
	pinMode(DS1,OUTPUT);
	pinMode(DS2,OUTPUT);
	NRF_init(TX_MODE, TX_buff, 14, 5);//MODE, TX_buff, TX_width, RX_width
}

void loop(void)
{
	digitalWrite(DS2,HIGH);
	digitalWrite(DS1,LOW);
	delay(100);
	printf("millis:%d \t micros:%d \r\n",millis(),micros());
	for(i=0;i<8;i++)
	{
		printf("ADC%d : %d\r\n",i,analogRead(i));
	}
	
	digitalWrite(DS2,LOW);
	digitalWrite(DS1,HIGH);
	delay(100);
}

//*****************MAIN******************//
int main(void)
{
	delay_init();
	ADCx_Init();
	setup();
	while(1)loop();
}
