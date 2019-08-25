#include "FileGroup.h"

MillisTaskManager mtm_Main(10);

Adafruit_ST7735 tft(TFT_CS,  TFT_DC, TFT_RST);

PageManager pm(PAGE_END);

void TaskManager_Loop();

void setup()
{
	Init_TriggerCtrl();
	Serial.begin(115200);
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextWrap(true);
	//delay(200);
    //Boot_logo2();
	//delay(1000);
	//nokia_tune(100,0.5); 

    //tft.drawRGBBitmap(10, 44, (const uint16_t*)loge, 137, 40);
    tft.clear();
    mtm_Main.TaskRegister(0, Task_SensorUpdate, 10);
    mtm_Main.TaskRegister(1, Task_LoadCtrlMode, 10);
    mtm_Main.TaskRegister(2, Task_TransferData, 10);
    mtm_Main.TaskRegister(3, Task_MPU6050Read, 20);

    TimerSet(TaskManager_Timer, TaskManager_Freq, TaskManager_Loop);
    TIM_Cmd(TaskManager_Timer, ENABLE);
}

void loop()
{
	//pm.Running();
	Serial.println("Hello.....");
}

void TaskManager_Loop()
{
    mtm_Main.Running(millis());
}


//*****************MAIN******************//
int main(void)
{
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
