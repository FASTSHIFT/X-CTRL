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
    if(State_DisplayCPU_Usage)
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

    if(State_LuaScript)
        Init_LuaScript();
    
    Init_GUI(1); //GUI初始化
}

/**
  * @brief  主线程
  * @param  无
  * @retval 无
  */
static void Thread_Main()
{
    Thread_GUI();//GUI线程
    //Thread_HMI();//HMI线程
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
