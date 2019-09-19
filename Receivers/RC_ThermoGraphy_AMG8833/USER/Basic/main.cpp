#include "FileGroup.h"

MillisTaskManager mtm_Main(2);

void Main_Loop();

static bool IsNRF_Enable = false;

void setup()
{
    pinMode(InfoLED_Pin, OUTPUT);
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, LOW);
    Init_ButtonEvent();
    Init_NRF();

    tft.initR(INITR_MINI160x80);
    tft.setRotation(3);
    tft.invertDisplay(false);
    tft.fillScreen(ST7735_BLACK);

    tft.setFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_RED, ST7735_BLACK);
    tft.setCursor(19, 45);
    tft.print("ThermoGraphy");
    tft.setFont();
    digitalWrite(TFT_BLK, HIGH);
    delay(1500);

    Serial.begin(115200);
    // default settings
    if (!amg.begin()) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }

    Serial.println("-- Thermal Camera Test --");

    mtm_Main.TaskRegister(0, Task_ButtonEventMonitor, 10);
    TimerSet(TIM1, 10 * 1000, Main_Loop);
    TIM_Cmd(TIM1, ENABLE);
    tft.fillScreen(ST7735_BLACK);
    
    IsNRF_Enable = !digitalRead(Button_UP_Pin);
}

void AMG_Loop()
{
    amg.readPixels(AMG_OriginPixels);//read all the pixels
    if(IsNRF_Enable)
    {
        NRF_SendPixels((uint8_t*)AMG_OriginPixels, sizeof(AMG_OriginPixels));
    }
    else
    {
        uint8_t NowResolutionRatio = ResolutionRatio[ResolutionRatio_Select];
        float *dest_2d = new float[NowResolutionRatio * NowResolutionRatio];
        interpolate_image(AMG_OriginPixels, AMG_ROWS, AMG_COLS, dest_2d, NowResolutionRatio, NowResolutionRatio);
        uint16_t boxsize = min(tft.width() / NowResolutionRatio, tft.height() / NowResolutionRatio);
        AMG_Drawpixels(dest_2d, NowResolutionRatio, NowResolutionRatio, boxsize, boxsize, true);
        delete[] dest_2d;
    }
    
    togglePin(InfoLED_Pin);
}

void Main_Loop()
{
    mtm_Main.Running(millis());
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    GPIO_JTAG_Disable();
    SysClock_Init(F_CPU_128MHz);
    Delay_Init();
//  ADCx_Init(ADC1);
    setup();
    for(;;)AMG_Loop();
}
