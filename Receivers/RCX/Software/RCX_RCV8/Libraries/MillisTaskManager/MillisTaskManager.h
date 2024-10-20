/*
 * MIT License
 * Copyright (c) 2018-2023 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
  ******************************************************************************
  * @file    MillisTaskManager.h
  * @author  _VIFEXTech
  * @version v2.1
  * @date    2020-7-23
  * @brief   超轻量级分时合作式任务调度器，可以替代旧的millis()轮询方案，不依赖ArduinoAPI
  * @Upgrade 2018.7.26  v1.0 初代
  * @Upgrade 2018.10.9  v1.1 将任务状态标志位类型换成bool类型
  * @Upgrade 2018.11.17 v1.2 将typedef转移至class内部
  * @Upgrade 2018.11.18 v1.3 修复50天后数值溢出导致任务停止的bug
  * @Upgrade 2018.12.17 v1.4 将 TaskCtrl 修改为 TaskStateCtrl，添加修改任务间隔时间的接口，添加 TaskFind 用于遍历列表寻找任务
  * @Upgrade 2019.2.5   v1.5 添加析构函数，用于释放内存
  * @Upgrade 2019.3.4   v1.6 将FuncPos改为ID，添加TaskFind(void_TaskFunction_t Function)
  * @Upgrade 2019.3.21  v1.7 支持设定优先级，优先级排列为任务ID号，数字越小优先级越高
  * @Upgrade 2019.4.24  v1.8 添加GetCPU_Useage()可获得CPU占用率
  * @Upgrade 2019.5.8   v1.9 为TaskRegister添加防冲突判断
  * @Upgrade 2019.9.3   v2.0 添加TimeCost任务时间开销计算
  * @Upgrade 2020.7.23  v2.1 使用单链表管理任务，添加 GetTickElaps 处理uint32溢出问题，添加时间误差记录
  * @Upgrade 2023.3.26  v2.2 完全重构，使用std::list进行管理，删除不常用功能
  ******************************************************************************
  * @attention
  * 需要提供一个精确到毫秒级的系统时钟，然后周期调用Running函数
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MILLIS_TASK_MANAGER_H
#define __MILLIS_TASK_MANAGER_H

#include <stdint.h>

namespace MTM {

class Manager;

class Task {
public:
    typedef void (*TaskFunction_t)(Task*);

public:
    Task(Manager* manager, TaskFunction_t taskFunc, uint32_t period, void* userData)
        : _manager(manager)
        , _taskFunc(taskFunc)
        , _userData(userData)
        , _period(period)
        , _lastTick(0)
        , _isPause(false)
    {
    }
    ~Task() { }
    void pause()
    {
        _isPause = true;
    }

    void resume()
    {
        _isPause = false;
    }

    void setPeriod(uint32_t period)
    {
        _period = period;
    }

    void* getUserData()
    {
        return _userData;
    }

    inline bool isReady();

    inline void invoke()
    {
        _taskFunc(this);
    }

    inline void resetTick(uint32_t tick)
    {
        _lastTick = tick;
    }

private:
    Manager* _manager;
    TaskFunction_t _taskFunc;
    void* _userData;
    uint32_t _period;
    uint32_t _lastTick;
    bool _isPause;
};

class Manager {
public:
    typedef uint32_t (*TickFunction_t)(void);

public:
    Manager(TickFunction_t tickCallback);
    ~Manager();

    Task* createTask(Task::TaskFunction_t func, uint32_t period, void* userData = nullptr);
    void deleteTask(Task* task);
    void handler();
    uint32_t getTickElaps(uint32_t prevTick);

private:
    TickFunction_t _getTick;
    void* _taskList;
};

} /* namespace MTM */

#endif /* __MILLIS_TASK_MANAGER_H */
