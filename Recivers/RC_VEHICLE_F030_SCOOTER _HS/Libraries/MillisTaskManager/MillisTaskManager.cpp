#include "MillisTaskManager.h"

//用途：初始化任务列表
//参数: (设定任务列表总长度)
MillisTaskManager::MillisTaskManager(TaskNum_t TaskNum_MAX_Set)
{
    if(TaskList != 0) delete TaskList;//清空列表
    TaskList = new MillisTaskManager_TypeDef[TaskNum_MAX_Set];//为任务列表申请内存
    TaskNum_MAX = TaskNum_MAX_Set;//记录任务列表总长度

    for(TaskNum_t i = 0; i < TaskNum_MAX; i++)//初始化任务列表，清零
    {
        TaskClear(i);
    }
}

//用途：不带越界判断地清除一个任务，private权限
//参数: (任务注册的位置)
void MillisTaskManager::TaskClear(TaskNum_t FuncPos)
{
    TaskList[FuncPos].Function = 0;
    TaskList[FuncPos].State = false;
    TaskList[FuncPos].IntervalTime = 0;
    TaskList[FuncPos].TimePoint = 0;
}

//用途：在任务列表内的一个位置注册一个任务，设定间隔执行时间
//参数: (任务注册地址，任务函数指针，时间设定(毫秒))
bool MillisTaskManager::TaskRegister(TaskNum_t FuncPos, void_TaskFunction_t Function, uint32_t TimeSetMs, bool TaskState)
{
    if(FuncPos < TaskNum_MAX)//判断是否越界
    {
        TaskList[FuncPos].Function = Function;//注册函数指针
        TaskList[FuncPos].State = TaskState;//初始状态设定
        TaskList[FuncPos].IntervalTime = TimeSetMs;//注册时间
        return true;//注册成功
    }
    else return false;//注册失败
}

//用途：注销任务
//参数: (任务函数指针)
bool MillisTaskManager::TaskLogout(void_TaskFunction_t Function)
{
    for(TaskNum_t i = 0; i < TaskNum_MAX; i++)
    {
        if(TaskList[i].Function == Function)
        {
            TaskClear(i);
            return true;
        }
    }
    return false;
}

//用途：注销任务
//参数: (任务注册的位置)
bool MillisTaskManager::TaskLogout(TaskNum_t FuncPos)
{
    if(FuncPos < TaskNum_MAX)
    {
        TaskClear(FuncPos);
        return true;
    }
    else return false;
}

//用途：任务状态控制
//参数: (任务函数指针，任务状态)
bool MillisTaskManager::TaskCtrl(void_TaskFunction_t Function, bool TaskState)
{
    for(TaskNum_t i = 0; i < TaskNum_MAX; i++)
    {
        if(TaskList[i].Function == Function)
        {
            TaskList[i].State = TaskState;
            return true;
        }
    }
    return false;
}

//用途：任务状态控制
//参数: (任务注册的位置，任务状态)
bool MillisTaskManager::TaskCtrl(TaskNum_t FuncPos, bool TaskState)
{
    if(FuncPos < TaskNum_MAX)
    {
        TaskList[FuncPos].State = TaskState;
        return true;
    }
    else return false;
}

//用途：执行调度器
//参数: (一个精确到毫秒的系统时钟)
void MillisTaskManager::Running(uint32_t MillisSeed)
{
    for(TaskNum_t i = 0; i < TaskNum_MAX; i++)//遍历任务列表
    {
        if(TaskList[i].Function)//判断是否为空函数
        {
            if(TaskList[i].State && (MillisSeed - TaskList[i].TimePoint >= TaskList[i].IntervalTime))//判断是否运行任务，是否到达触发时间点
            {
                TaskList[i].TimePoint = MillisSeed;//标记下一个时间点
                TaskList[i].Function();//执行任务
            }
        }
    }
}
