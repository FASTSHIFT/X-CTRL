//Designed By _VIFEXTech
//Finish 2018.3.5       V1.0 引入结构体保存数据，使用新版本的ArduinoAPI
//Upgrade  4.2 			V1.1 更新ArduinoAPI
//Upgrade  5.18 		V1.2 使用结构体保存通信协议
//Upgrade  6.22 		V1.3 整理代码
//Upgrade  6.23 		V1.4 优化tone()
//Upgrade  6.27 		V1.5 硬件电路完成,电压监控已支持,MPU6050已支持
//Upgrade  6.28 		V1.6 优化电量显示
//Upgrade  6.29 		V1.7 改进Wire库，刷屏速度翻倍
//Upgrade  6.30 		V1.8 改进信息显示蜂鸣器效果，支持MPU挂载任意摇杆，使用开关来切换黑白反显
//Upgrade  7.1			V1.9 用DMA读取ADC，获得更稳定的信号，修复花屏，加入主循环运行速度监控
//Upgrade  7.4			V2.0 更新Print库，优化按键读取，优化提示音效果，加入页面锁定，修复后退键的BUG
//Upgrade  7.7			V2.1 使用新的Print库，支持 << 流式操作，改进Mode显示

#include "FileGroup.h"

CtrlOption_TypeDef CTRL;

void setup()
{
    Wire.begin();
    Pin_Config();
    mpu.initialize();
    tft.initR(INITR_MINI128x64);
    tft.fillScreen(ST7735_BLACK);

    digitalWrite(BT_PowerCtrl_Pin, HIGH);
    pwmWrite(EncoderLED_Pin, 0);

    Silent_Mode = !digitalRead(EncoderKey_Pin);

    DataStorageHandle(ReadData);

    if(CTRL.Baud_Select > 10)						CTRL.Baud_Select = UART_Baud_Select_Default;
    if(CTRL.SendFreq < 5 || CTRL.SendFreq > 50000)	CTRL.SendFreq = SendFreq_Default;
    if(CTRL.Mode < 1 || CTRL.Mode > 3)				CTRL.Mode = CtrlMode_Default;
    if(CTRL.Object > CRAWLER)						CTRL.Object = CtrlObject_Default;

    if (CTRL.Pattern == CtrlPattern_NRF)
    {
        Init_NRF();
    }
    KeyClear();

    BlueTooth.begin(UART_Baud[CTRL.Baud_Select]);
    TimerSet(TIM2, CTRL.SendFreq * 1000, SendData);
    TimerSet(TIM4, LoopFreq_Default, LoopExecute);
    LoopExecute_Switch(ENABLE);
    SendData_Switch(ENABLE);

    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
    tft.setTextWrap(false);
    tft.setTextSize(1);
    tft.drawRect(0, 8, 50, 50, ST7735_WHITE);
    tft.drawRect(127 - 50, 8, 50, 50, ST7735_WHITE);
}

void loop()
{
    static uint8_t L_Circle_X_Last, L_Circle_Y_Last;
    static uint8_t R_Circle_X_Last, R_Circle_Y_Last;

    tft.setCursor(0, 0);
    tft << MainLoop_Speed << "FPS ";

    uint8_t L_Circle_X = map(JS_L.X, -255, 255, 6, 43);
    uint8_t L_Circle_Y = map(-JS_L.Y, -255, 255, 14, 51);
    uint8_t R_Circle_X = map(JS_R.X, -255, 255, 127 - 50 + 6, 127 - 7);
    uint8_t R_Circle_Y = map(-JS_R.Y, -255, 255, 14, 51);

    if(L_Circle_X != L_Circle_X_Last || L_Circle_Y != L_Circle_Y_Last)
    {
        //tft.fillRect(0+1, 8+1, 50-2, 50-2, ST7735_BLACK);
        tft.fillCircle(L_Circle_X_Last, L_Circle_Y_Last, 5, ST7735_BLACK);
        tft.fillCircle(L_Circle_X, L_Circle_Y, 5, ST7735_WHITE);
        L_Circle_X_Last = L_Circle_X;
        L_Circle_Y_Last = L_Circle_Y;
    }
    if(R_Circle_X != R_Circle_X_Last || R_Circle_Y != R_Circle_Y_Last)
    {
        //tft.fillRect(127 - 50 +1, 8+1, 50-2, 50-2, ST7735_BLACK);
        tft.fillCircle(R_Circle_X_Last, R_Circle_Y_Last, 5, ST7735_BLACK);
        tft.fillCircle(R_Circle_X, R_Circle_Y, 5, ST7735_WHITE);
        R_Circle_X_Last = R_Circle_X;
        R_Circle_Y_Last = R_Circle_Y;
    }

    LoopSpeedDetect();
}

//*****************MAIN******************//
int main()
{
    Delay_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
