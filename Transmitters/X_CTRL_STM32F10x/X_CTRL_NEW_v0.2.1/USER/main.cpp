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

MillisTaskManager mtm(10);

void TaskManagerRun()
{
    mtm.Running(millis());
}

void setup()
{
	Init_X_CTRL();
	
    mtm.TaskRegister(0, Task_SensorUpdate, 5);
    mtm.TaskRegister(1, Task_LoadCtrlMode, 5);
    mtm.TaskRegister(2, Task_TransferData, 10);
    TimerSet(TaskManager_Timer, TaskManager_Freq, TaskManagerRun);
    TIM_Cmd(TaskManager_Timer, ENABLE);
}

void loop()
{
	Task_TFT_Display();
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
