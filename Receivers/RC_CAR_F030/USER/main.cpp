#include "FileGroup.h"

//#define USE_DEBUG

MillisTaskManager mtm(8);

#ifdef USE_DEBUG
void Task_Test()
{
    Serial << MotorSpeed << " " << ServoAngle <<"\r\n";
}
#endif

void setup()
{
	toneSetTimer(TIM14);
    pinMode(Buzz_Pin, OUTPUT); 
    toneBlock(Buzz_Pin, 523, 80);
    toneBlock(Buzz_Pin, 880, 80);
    toneBlock(Buzz_Pin, 659, 80);
	
	PWM_Init(LED_Pin, 100, 100);
	
    nrf.SetAddress(NRF_Address_CarSS);
    nrf.init(RX_MODE, sizeof(RC_CAR));
    while(!NRF_Detect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
    }

    delay(1500);
    BrushlessMotor.attach(Motor_Pin);
    SteerServo.attach(Servo_Pin);

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
