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
#include "MillisTaskManager.h"
#include <list>

#ifndef UINT32_MAX
#define UINT32_MAX 4294967295u
#endif

namespace MTM {

typedef std::list<Task*> TaskList_t;
#define TASK_LIST (((TaskList_t*)_taskList))

bool Task::isReady()
{
    if (_isPause) {
        return false;
    }

    if (_manager->getTickElaps(_lastTick) < _period) {
        return false;
    }

    return true;
}

Manager::Manager(TickFunction_t tickCallback)
{
    _getTick = tickCallback;
    _taskList = new TaskList_t;
}

Manager::~Manager()
{
    TaskList_t& list = *TASK_LIST;
    for (auto& iter : list) {
        deleteTask(iter);
    }

    delete (TaskList_t*)_taskList;
}

Task* Manager::createTask(Task::TaskFunction_t func, uint32_t period, void* userData)
{
    Task* task = new Task(this, func, period, userData);
    TASK_LIST->push_back(task);
    return task;
}

void Manager::deleteTask(Task* task)
{
    TASK_LIST->remove(task);
    delete task;
}

void Manager::handler()
{
    TaskList_t& list = *TASK_LIST;
    for (const auto& iter : list) {
        if (iter->isReady()) {
            iter->resetTick(_getTick());
            iter->invoke();
        }
    }
}

uint32_t Manager::getTickElaps(uint32_t prevTick)
{
    uint32_t act_time = _getTick();

    /*If there is no overflow in sys_time simple subtract*/
    if (act_time >= prevTick) {
        prevTick = act_time - prevTick;
    } else {
        prevTick = UINT32_MAX - prevTick + 1;
        prevTick += act_time;
    }

    return prevTick;
}

} /* namespace MTM */
