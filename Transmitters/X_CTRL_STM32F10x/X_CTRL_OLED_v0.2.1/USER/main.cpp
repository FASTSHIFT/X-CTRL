//Designed By _VIFEXTech
//Finish 2018.3.5   V1.0 引入结构体保存数据，使用新版本的ArduinoAPI
//Upgrade  4.2          V1.1 更新ArduinoAPI
//Upgrade  5.18         V1.2 使用结构体保存通信协议
//Upgrade  6.22         V1.3 整理代码
//Upgrade  6.23         V1.4 优化tone()
//Upgrade  6.27         V1.5 硬件电路完成,电压监控已支持,MPU6050已支持
//Upgrade  6.28         V1.6 优化电量显示
//Upgrade  6.29         V1.7 改进Wire库，刷屏速度翻倍
//Upgrade  6.30         V1.8 改进信息显示蜂鸣器效果，支持MPU挂载任意摇杆，使用开关来切换黑白反显
//Upgrade  7.1          V1.9 用DMA读取ADC，获得更稳定的信号，修复花屏，加入主循环运行速度监控
//Upgrade  7.4          V2.0 更新Print库，优化按键读取，优化提示音效果，加入页面锁定，修复后退键的BUG
//Upgrade  7.7          V2.1 使用新的Print库，支持 << 流式操作，改进Mode显示

#include "FileGroup.h"

CtrlOption_TypeDef CTRL;

void setup()
{
    Wire.begin();
    Pin_Config();
    mpu.initialize();

    digitalWrite(BT_PowerCtrl_Pin, HIGH);
    pwmWrite(EncoderLED_Pin, 0);

    Silent_Mode = !digitalRead(EncoderKey_Pin);

    oled.init(0x3C);
    oled.setBrightness(0);
    oled.drawFullScreen(X_CTRL_LOGO, sizeof(X_CTRL_LOGO));
    for(uint8_t i = 0; i < 100; i++)
    {
        oled.setBrightness(i);
        pwmWrite(EncoderLED_Pin, i * 2);
        delay(20);
    }
//  oled.setHorizontalScrollProperties(Scroll_Left,0,7,Scroll_2Frames);
//  oled.activateScroll();
//  delay(3000);
//  oled.deactivateScroll();

    oled.clear();
    OLED_DynamicPrint(1, 0, "Loading...", 2);
    DataStorageHandle(ReadData);

    if(CTRL.Baud_Select > 10)                                           CTRL.Baud_Select = UART_Baud_Select_Default;
    if(CTRL.SendFreq < 5 || CTRL.SendFreq > 50000)CTRL.SendFreq      = SendFreq_Default;
    if(CTRL.Mode < 1 || CTRL.Mode > 3)                      CTRL.Mode            = CtrlMode_Default;
    if(CTRL.Object > CRAWLER)                                           CTRL.Object          = CtrlObject_Default;

    oled.setCursor(1, 1), oled << "Send fq:" << CTRL.SendFreq << "ms";
    BuzzRing(5);
    oled.setCursor(1, 2), oled << "Mode:" << CTRL.Mode;
    BuzzRing(5);
    oled.setCursor(1, 3), oled.print("Object:");
    if (CTRL.Object == CAR)oled.print("CAR ");
    else if (CTRL.Object == TANK)oled.print("TANK");
    else if (CTRL.Object == CPTR)oled.print("CPTR");
    else if (CTRL.Object == SPIDER)oled.print("SPID");
    BuzzRing(5);

    oled.setCursor(1, 4), oled.print("Pattern:");
    if (CTRL.Pattern == CtrlPattern_NRF)oled.print("2.4G");
    else oled.print("UART");
    BuzzRing(5);
    oled.setCursor(1, 5), oled << "Baud:" << UART_Baud[CTRL.Baud_Select];
    BuzzRing(5);
    delay(1500);

    if (CTRL.Pattern == CtrlPattern_NRF)
    {
        Init_NRF();
    }
    oled.clear();
    KeyClear();

    Serial.begin(UART_Baud[CTRL.Baud_Select]);
    TimerSet(TIM2, CTRL.SendFreq * 1000, SendData);
    oled.setCacheMode(On);
    TimerSet(TIM4, DisplayFreq_Default, OLED_Refresh_Handler);
    OLED_Display(ENABLE);
}

void loop()
{
    if (CTRL.Pattern == CtrlPattern_UART && Serial.available() >= RX_DataLength)
    {
        RX_DataLoad();
    }
    else if (CTRL.Pattern == CtrlPattern_NRF)
    {
        NRF_CheckConnection();
        RX_Transform();
    }

    Read_Joystick();
    Read_Keys();
    if(MPU_Switch == On) Read_MPU6050(MPU_ReadFreq);

    if(MenuState == INF)
    {
        LoadCtrlMode(CTRL.Mode);
        TX_DataLoad();
    }
    MenuRun();
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
