/** @Author: _VIFEXTech
  * @Describe: Time shareing operating system, base on <X-CTRL> Framework.
  */

#include "FileGroup.h"

/*主调度器优先级分配表*/
enum TaskPriority {
    TP_ClacSystemUsage,
    TP_SensorUpdate,
    TP_MotorRunning,
    TP_MusicPlayerRunning,
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
    CPU_Usage = ControlTask.GetCPU_Usage() * 100;
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
    ControlTask.TaskRegister(TP_SensorUpdate,       Task_SensorUpdate,          5);     //传感器读取任务，执行周期5ms
    ControlTask.TaskRegister(TP_MotorRunning,       Task_MotorRunning,          10);    //电机振动任务，执行周期10ms
    ControlTask.TaskRegister(TP_MusicPlayerRunning, Task_MusicPlayerRunning,    20);    //音乐播放任务，，执行周期20ms
    ControlTask.TaskRegister(TP_ClacSystemUsage,    Task_ClacCPU_Usage,     	1000);
    Timer_Init(TIM_ControlTask, 1000, Thread_Control, 1, 1);  //主调度器(控制线程)与硬件定时器绑定，时间片1ms，主优先级1，从优先级1
    TIM_Cmd(TIM_ControlTask, ENABLE);                         //定时器使能

    Init_GUI(); //GUI初始化

    Init_SD();
    
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
    ADCx_DMA_Init();                               //ADC DMA 初始化
    //ADCx_Init(ADC1);
    SystemSetup();                                  //系统初始化
    for(;;)Thread_Main();                           //主线程执行
}
