/** @Author: _VIFEXTech
  * @Describe: <X-CTRL> A brand new high performance digital wireless remote control system.
  * @Finished: 2018.10.6  - v1.0 全新的任务调度系统、事件驱动机制以及GUI框架
  * @Upgrade:  2018.10.12 - v1.1 增加HMI
  * @Upgrade:  2018.10.14 - v1.2 增加解锁开关以及HMI交互
  * @Upgrade:  2018.10.19 - v1.3 更新蓝牙主从控制
  * @Upgrade:  2018.10.23 - v1.4 修复蓝牙设置应用失败的BUG
  * @Upgrade:  2018.10.26 - v1.5 增加MPU6050姿态解算，提升重力感应响应速度
  * @Upgrade:  2018.11.17 - v1.6 同步RC-CTRL代码，支持MPU挂载任意摇杆，添加HMI开关控制，调整GUI项目序号
  * @Upgrade:  2018.11.21 - v1.7 优化HMI交互
  * @Upgrade:  2018.11.23 - v1.8 MainMenu加入动画跳过效果，支持调整NRF地址，增加失控临界警告
  * @Upgrade:  2018.11.26 - v1.9 修改连接提示音
  * @Upgrade:  2018.11.28 - v2.0 添加通用回传协议,合并Task_LoadCtrlMode 至 Task_TransferData
  * @Upgrade:  2018.11.30 - v2.1 支持设置NRF的频道、空中速率
  * @Upgrade:  2018.12.17 - v2.2 超频至112MHz
  * @Upgrade:  2018.12.18 - v2.3 弃用外部EEPROM，使用FLASH储存设置参数，超频至128MHz
  * @Upgrade:  2018.12.23 - v2.4 更新按键事件驱动库
  * @Upgrade:  2018.12.29 - v2.5 添加Handshake握手协议
  * @Upgrade:  2019.1.26  - v2.6 更新底层库
  * @Upgrade:  2019.2.5   - v2.7 使用更快速的刷图函数
  * @Upgrade:  2019.2.10  - v2.8 降低屏幕驱动程序与GUI程序的耦合度
  */
  
#include "FileGroup.h"

MillisTaskManager mtm_Main(4);
void TaskManager_Loop();

void setup()
{  
    Init_X_CTRL();
    
    mtm_Main.TaskRegister(0, Task_SensorUpdate, 5);
    mtm_Main.TaskRegister(1, Task_TransferData, SendFreq_Default);
    mtm_Main.TaskRegister(2, Task_EncoderLED, 30);
    mtm_Main.TaskRegister(3, Task_MPU6050Read, MPU_ReadFreq_Default);
    Timer_Init(TaskManager_Timer, TaskManager_Freq, TaskManager_Loop, 1, 1);

    Init_GUI();

    TIM_Cmd(TaskManager_Timer, ENABLE);
}

void loop()
{
    mtm_StatusBar.Running(millis());
    pm.Running();
    HMI_Running();
}

void TaskManager_Loop()
{
    mtm_Main.Running(millis());
}


/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SysClock_Init(F_CPU_128MHz);
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
