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
  * @Upgrade:  2019.2.11  - v2.9 加入MenuManager菜单管理框架，简化菜单的逻辑设计，添加布尔开关控件，简化旋转编码器事件处理
  * @Upgrade:  2019.2.15  - v3.0 加入XFS语音模块支持，移除蓝牙作为遥控的选项
  * @Upgrade:  2019.2.16  - v3.1 添加模版类DigitalFilter、FifoQueue，支持泛型
  * @Upgrade:  2019.2.17  - v3.2 整理代码，优化电池载入动画，Handshake页面添加滚动条控件
  * @Upgrade:  2019.2.18  - v3.3 更新GUI控件库(LightGUI)
  * @Upgrade:  2019.2.20  - v3.4 增加非阻塞式PageDelay
  * @Upgrade:  2019.2.21  - v3.5 修复回传信息更新与显示线程锁死的BUG
  * @Upgrade:  2019.2.28  - v3.6 加入CommonMacro通用宏定义库
  * @Upgrade:  2019.2.28  - v3.7 优化握手超时处理
  * @Upgrade:  2019.3.8   - v3.8 优化IMU处理，降低主程序与GUI层的耦合度
  * @Upgrade:  2019.3.9   - v3.9 添加省电模式
  * @Upgrade:  2019.3.15  - v4.0 更新NRF库，在启用握手后已禁止修改频率和通信速度，修复退出握手后NRF依然工作的BUG
  * @Upgrade:  2019.3.16  - v4.1 修复蓝牙连接状态判断的BUG
  * @Upgrade:  2019.3.20  - v4.2 更优雅的数据储存方式
  * @Upgrade:  2019.3.22  - v4.4 更新NRF库，增强通信环境较差时的通信稳定性，增加NRF收发模式显示
  * @Upgrade:  2019.4.1   - v4.5 更新页面调度器以及LightGUI库，更新音频并行播放器
  */
  
#include "FileGroup.h"

static MillisTaskManager ControlTask(5, true);
static void Thread_Control();

static void SystemSetup()
{
    Init_X_CTRL();

    ControlTask.TaskRegister(0, Task_SensorUpdate, 10);
    ControlTask.TaskRegister(1, Task_TransferData, 10);
    ControlTask.TaskRegister(2, Task_MPU6050Read, 10);
    ControlTask.TaskRegister(3, Task_MusicPlayerRunning, 20);
    ControlTask.TaskRegister(4, Task_XFS_ListCheck, 500);
    
    Timer_Init(TIM3, 25*1000, Task_ScreenDisplay, 2, 0);
    Timer_Init(TaskManager_Timer, 1000, Thread_Control, 1, 0);
    TIM_Cmd(TaskManager_Timer, ENABLE);

    Init_GUI();

    Init_XFS();
    XFS_Speak("系统已就绪");
}

static void Thread_Main()
{
    Thread_GUI();
    Thread_HMI();
}

void Thread_Control()
{
    ControlTask.Running(millis());
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    GPIO_JTAG_Disable();
    Delay_Init();
    //SysClock_Config();
    ADCx_DMA_Init();
    SystemSetup();
    for(;;)Thread_Main();
}
