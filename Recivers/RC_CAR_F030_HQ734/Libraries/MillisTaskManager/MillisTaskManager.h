//Designed By _VIFEXTech 
//超轻量级分时任务调度器

//Finished in 2018.7.26 v1.0 可以替代旧的millis()循环判断的方案，不依赖ArduinoAPI
//Update           10.9 v1.1 将任务状态标志位类型换成bool类型

#ifndef __MILLISTASKMANAGER_H
#define __MILLISTASKMANAGER_H

#include "stdint.h"

typedef void(*void_TaskFunction_t)(void);//函数指针类型定义
typedef uint8_t TaskNum_t;

typedef struct{
	bool State; //任务状态
	void_TaskFunction_t Function; //任务函数指针
	uint32_t IntervalTime; //任务触发间隔时间
	uint32_t TimePoint; //任务触发时间点
}MillisTaskManager_TypeDef;//任务类型定义

class MillisTaskManager{
	public:
		MillisTaskManager(TaskNum_t TaskNum_MAX_Set);
		bool TaskRegister(TaskNum_t FuncPos, void_TaskFunction_t Function, uint32_t TimeSetMs, bool TaskState = true);
		bool TaskLogout(void_TaskFunction_t Function);
		bool TaskLogout(TaskNum_t FuncPos);
		bool TaskCtrl(void_TaskFunction_t Function, bool TaskState);
		bool TaskCtrl(TaskNum_t FuncPos, bool TaskState);
		void Running(uint32_t MillisSeed);
	private:
		MillisTaskManager_TypeDef* TaskList;//任务列表
		TaskNum_t TaskNum_MAX;//任务列表长度
		void TaskClear(TaskNum_t FuncPos);//清除任务
};

#endif
