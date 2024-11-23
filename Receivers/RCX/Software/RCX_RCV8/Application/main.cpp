#include "FileGroup.h"
#include "ComPrivate.h"

MTM::Manager mtm(millis);

static void Task_WatchDogFeed(MTM::Task* task)
{
    WDG_ReloadCounter();
}

static void setup()
{
    Serial.begin(115200);
    pinMode(LED_Pin, OUTPUT);
    __LoopExecute((togglePin(LED_Pin), delay(80)), 6);

    ADCx_Init(ADC1);
    pinMode(BAT_DET_Pin, INPUT_ANALOG);
//    ADC_DMA_Init();
//    randomSeed(analogRead(BAT_DET_Pin));

    Com_Init();
    Init_MotorServo();
    
    mtm.createTask(Task_ComHandler, 10);
    mtm.createTask(Task_Ctrl, 5);
    mtm.createTask(Task_InfoLED, 500);
    mtm.createTask(Task_ComPassback, 1000);
    mtm.createTask(Task_WatchDogFeed, 500);

    while (!RCX::Handshake::Process())
    {
        __IntervalExecute(togglePin(LED_Pin), 2000);
    }

    Com_PassbackInit();
    WDG_SetTimeout(1000);
    WDG_SetEnable();
}

static void loop()
{
    mtm.handler();
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    Core_Init();
    setup();
    for(;;)loop();
}
