#include "FileGroup.h"

/*实例化主调度器对象，开启优先级*/
MillisTaskManager MainTask(TP_MAX, true);

/*主线程*/
static void MainTask_Process();

/**
  * @brief  系统初始化
  * @param  无
  * @retval 无
  */
static void SystemSetup()
{
    Serial.begin(115200);
    Serial.println(_X_CTRL_NAME","_X_CTRL_VERSION","__DATE__","__TIME__);
    Init_X_CTRL();  //初始化遥控器

    /*主调度器任务注册*/
    MainTask.TaskRegister(TP_TransferData,       Task_TransferData,          10);    //数据发送任务，执行周期10ms
    MainTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          10);    //传感器读取任务，执行周期10ms
    MainTask.TaskRegister(TP_IMU_Process,        Task_IMU_Process,           20);    //姿态解算任务，执行周期20ms
    MainTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //电机振动任务，执行周期10ms
    MainTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //音乐播放任务，执行周期20ms
    MainTask.TaskRegister(TP_XFS_ListCheck,      Task_XFS_ListCheck,         500);   //语音合成队列扫描任务，执行周期500ms
    MainTask.TaskRegister(TP_CPUInfoUpdate,      Task_CPUInfoUpdate,         1000);
    Timer_SetInterruptBase(TIM_ControlTask, 10, 10, MainTask_Process, 1, 1);  //主调度器(控制线程)与硬件定时器绑定，主优先级1，从优先级1
    Timer_SetInterruptTimeUpdate(TIM_ControlTask, 1000); //时间片1ms
    TIM_Cmd(TIM_ControlTask, ENABLE);//定时器使能

    Init_GUI(0); //LOGO

    if(Init_SD())
        Init_BvPlayer();

    if(CTRL.State.LuaScript)
        Init_LuaScript();
    
    Init_GUI(1); //GUI初始化
}

/**
  * @brief  主线程(主调度器)
  * @param  无
  * @retval 无
  */
static void MainTask_Process()
{
    MainTask.Running(millis());
}

/**
  * @brief  副线程
  * @param  无
  * @retval 无
  */
static void SubTask_Process()
{
    Thread_GUI();//GUI线程
    Thread_SD_Monitor();//SD热插拔监控线程
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    SystemSetup();
    for(;;)SubTask_Process();
}
