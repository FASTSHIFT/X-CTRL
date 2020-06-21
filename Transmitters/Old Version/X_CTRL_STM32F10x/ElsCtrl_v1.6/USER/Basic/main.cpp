/** @Author: _VIFEXTech
  * @Describe: <ElsCtrl> Electric long skateboard wireless controller, base on <X-CTRL> Framework. 
  * @Finished: 2019.3.26 v1.0 拥有基本的通信功能以及简单的GUI页面，支持定速巡航
  * @Upgrade:  2019.3.28 v1.1 为摇杆添加滤波器，支持摇杆校准，掉电保存数据，自动熄屏
  * @Upgrade:  2019.4.3  v1.2 在熄屏时可以一键退出定速巡航模式，优化待选从机列表高亮显示
  * @Upgrade:  2019.4.8  v1.3 优化开机动画跳过快捷键，将握手修改为单次模式，握手成功后注册机将握手地址频道信息保持至内部FLASH绑定
  * @Upgrade:  2019.6.27 v1.4 在熄屏后自动关闭刷屏定时器以节省电量,更新长时间无操作提示音
  * @Upgrade:  2019.7.7  v1.5 添加速度和距离回传
  * @Upgrade:  2019.7.8  v1.6 更新滤波器
  */
  
#include "FileGroup.h"

static MillisTaskManager ControlTask(3, true);
static void Thread_Control();

static void SystemSetup()
{
    Init_ElsCtrl();
    
    Timer_Init(TIM_DisplayThread, 25 * 1000/*25ms@40FPS*/, Task_ScreenDisplay, 2, 0);
    TIM_Cmd(TIM_DisplayThread, ENABLE);

    ControlTask.TaskRegister(0, Task_TransferData, 10);
    ControlTask.TaskRegister(1, Task_SensorUpdate, 10);
    ControlTask.TaskRegister(2, Task_MusicPlayerRunning, 20);    
    Timer_Init(TIM_ControlThread, 1000, Thread_Control, 1, 0);    
    TIM_Cmd(TIM_ControlThread, ENABLE);

    Init_GUI();
}

static void Thread_Main()
{
    Thread_GUI();
}

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    GPIO_JTAG_Disable();
    Delay_Init();
//    ADCx_Init(ADC1);
    ADCx_DMA_Init();
    SystemSetup();
    for(;;)Thread_Main();
}
