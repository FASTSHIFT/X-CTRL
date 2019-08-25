#include "FileGroup.h"

//#define USE_DEBUG

MillisTaskManager mtm(8);

#ifdef USE_DEBUG
void Task_Test()
{
	for(int i=0;i<sizeof(NRF_RxBuff);i++)
	{
		Serial.print(NRF_RxBuff[1],HEX);
	}
	Serial.println();
}
#endif

void setup()
{	
	PWM_Init(LED_Pin, 100, 1000);
	PWM_Init(MotorL_Pin,1000,1000);
	PWM_Init(MotorR_Pin,1000,1000);
	analogWrite(MotorL_Pin, MotorSpeed_MID);
    analogWrite(MotorR_Pin, MotorSpeed_MID);
	
	pinMode(MotorL_1_Pin,OUTPUT);
	pinMode(MotorL_2_Pin,OUTPUT);
	pinMode(MotorR_1_Pin,OUTPUT);
	pinMode(MotorR_2_Pin,OUTPUT);
	
	toneSetTimer(TIM14);
    pinMode(Buzz_Pin, OUTPUT); 
    toneBlock(Buzz_Pin, 523, 80);
    toneBlock(Buzz_Pin, 880, 80);
    toneBlock(Buzz_Pin, 659, 80);
	
    nrf.SetAddress(NRF_AddressConfig);
    nrf.init(RX_MODE, sizeof(NRF_RxBuff));
    while(!NRF_Detect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
    }

    delay(1500);

	pinMode(PA4, OUTPUT);	
    mtm.TaskRegister(0, Task_NRF_Receive, 10);
    mtm.TaskRegister(1, Task_CarCtrl, 5);
	mtm.TaskRegister(2, Task_BuzzRing_Block, 100);
	mtm.TaskRegister(3, Task_MID_Setting, 100);

#ifdef USE_DEBUG
    Serial.begin(115200);
	mtm.TaskRegister(4, Task_Test, 30);
#endif
}

void loop()
{
    mtm.Running(millis());
}

//*****************MAIN******************//

int main(void)
{
    Delay_Init();
//  ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
