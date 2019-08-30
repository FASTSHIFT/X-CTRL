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
  * @Upgrade:  2019.3.21  - v4.3 更新MTM库，支持设定优先级
  * @Upgrade:  2019.3.22  - v4.4 更新NRF库，增强通信环境较差时的通信稳定性，增加NRF收发模式显示
  * @Upgrade:  2019.4.1   - v4.5 更新页面调度器以及LightGUI库，更新音频并行播放器
  * @Upgrade:  2019.4.16  - v4.6 整理总框架，添加注释，更新NRF库提升通信稳定性
  * @Upgrade:  2019.4.21  - v4.7 X-CTRL框架已移植到X-CTRL GUN(STM32F405RGT6)
  * @Upgrade:  2019.4.22  - v4.8 添加Lua解释器，SD文件系统移植完成
  * @Upgrade:  2019.4.24  - v4.9 添加CPU占用率计算
  * @Upgrade:  2019.4.28  - v5.0 支持模拟XBox360振动回传
  * @Upgrade:  2019.4.29  - v5.1 支持用户添加自定义数据包格式
  * @Upgrade:  2019.4.30  - v5.2 添加菜单选中闪烁效果
  * @Upgrade:  2019.5.1   - v5.3 支持动态菜单图标，支持抗锯齿字体
  * @Upgrade:  2019.5.3   - v5.4 内部Flash储存数据已支持，添加主菜单字体渐变特效，ADC数据读取调整为DMA模式
  * @Upgrade:  2019.6.18  - v5.5 更新按键事件库
  * @Upgrade:  2019.6.20  - v5.6 更新Searching动画，添加Arduboy框架，在About页面加入彩蛋小游戏
  * @Upgrade:  2019.6.21  - v5.7 支持Lua控制按键状态
  * @Upgrade:  2019.6.22  - v5.8 支持掉电保存握手信息
  * @Upgrade:  2019.6.23  - v5.9 修复按键设置变量总是增加的BUG
  * @Upgrade:  2019.6.28  - v6.0 支持设置摇杆非线性拟合曲线
  * @Upgrade:  2019.6.29  - v6.1 添加滞回滤波器，修复电池电量抖动的BUG
  * @Upgrade:  2019.6.30  - v6.2 修复图形库在屏幕负半轴运算产生的BUG
  * @Upgrade:  2019.7.3   - v6.3 提升UI分辨率自适应能力，增加控件渐变色效果
  * @Upgrade:  2019.7.6   - v6.4 修改动态菜单效果，根据SD卡插入情况自动切换主页面，修复CPU占用率显示超过100%的BUG，添加CPU温度显示
  * @Upgrade:  2019.7.9   - v6.5 修复语音模块通信失败的BUG
  * @Upgrade:  2019.7.13  - v6.6 修复摇杆负行程非线性映射斜率反向错误的BUG
  * @Upgrade:  2019.7.15  - v6.7 添加充电口插入和拔出提示音
  * @Upgrade:  2019.7.16  - v6.8 优化BuzzMusic以及开机动画，修复握手页面滚动条无法自适应屏幕分辨率的BUG
  * @Upgrade:  2019.7.17  - v6.9 移除PowerSaving，对齐PageJump标签
  * @Upgrade:  2019.7.18  - v7.0 添加SD卡热插拔支持，添加设备插入拔出提示音
  * @Upgrade:  2019.7.19  - v7.1 修复堆栈一调大ADC就停止工作的问题，要优先使用IRAM1内存！
  * @Upgrade:  2019.7.20  - v7.2 添加频谱扫描功能，添加硬件随机数支持，修复重新握手时缓冲区未清空的BUG
  * @Upgrade:  2019.7.23  - v7.3 在WString库中添加sprintf成员支持(测试版)
  * @Upgrade:  2019.7.26  - v7.4 优化蓝牙设置退出方式，支持不保存退出
  * @Upgrade:  2019.8.3   - v7.5 微调状态栏自适应布局，锁定开关宏，调整控制页面
  * @Upgrade:  2019.8.22  - v7.6 优化随机数生成，添加长时间无操作报警
  * @Upgrade:  2019.8.23  - v7.7 更新底层库，ADC DMA支持注册模式
  * @Upgrade:  2019.8.28  - v7.8 About页面已支持自适应屏幕分辨率，添加简易文件浏览器，添加WAV播放器
  */

#include "FileGroup.h"

/*主调度器优先级分配表*/
enum TaskPriority
{
    TP_ClacSystemUsage,
    TP_SensorUpdate,
    TP_TransferData,
    TP_MPU6050Read,
    TP_MotorRunning,
    TP_MusicPlayerRunning,
    TP_XFS_ListCheck,
    TP_MAX
};

/*实例化主调度器对象，开启优先级*/
static MillisTaskManager ControlTask(TP_MAX, true);

/*控制线程*/
static void Thread_Control();

/*计算CPU占用情况*/
float CPU_Usage;
static void Task_ClacCPU_Usage()
{
    CPU_Usage = ControlTask.GetCPU_Usage();
}

/**
  * @brief  系统初始化
  * @param  无
  * @retval 无
  */
static void SystemSetup()
{
    Init_X_CTRL();  //初始化遥控器

    /*主调度器任务注册*/
    ControlTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          10);    //传感器读取任务，执行周期10ms
    ControlTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //数据发送任务，执行周期10ms
    ControlTask.TaskRegister(TP_MPU6050Read,        Task_MPU6050Read,           20);    //姿态解算任务，执行周期20ms
    ControlTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //电机振动任务，执行周期10ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //音乐播放任务，执行周期20ms
    ControlTask.TaskRegister(TP_XFS_ListCheck,      Task_XFS_ListCheck,         500);   //语音合成队列扫描任务，优先级4，执行周期500ms
    ControlTask.TaskRegister(TP_ClacSystemUsage,    Task_ClacCPU_Usage,         1000);
    Timer_Init(TIM_ControlTask, 1000, Thread_Control, 1, 1);  //主调度器(控制线程)与硬件定时器绑定，时间片1ms，主优先级1，从优先级1
    TIM_Cmd(TIM_ControlTask, ENABLE);                         //定时器使能

    Init_GUI(0); //LOGO + HMI

    if(Init_SD())
        Init_BvPlayer();

    Init_GUI(1); //GUI初始化

    if(State_LuaScript)
        Init_LuaScript();
}

/**
  * @brief  主线程
  * @param  无
  * @retval 无
  */
static void Thread_Main()
{
    Thread_GUI();//GUI线程
    Thread_HMI();//HMI线程
    Thread_SD_Monitor();//SD热插拔监控线程
}


/**
  * @brief  控制线程(主调度器)
  * @param  无
  * @retval 无
  */
static void Thread_Control()
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //NVIC中断组设置
    GPIO_JTAG_Disable();                            //关闭JTAG
    Delay_Init();
    SystemSetup();                                  //系统初始化
    for(;;)Thread_Main();                           //主线程执行
}
