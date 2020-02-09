#include "FileGroup.h"
#include "DigitalFilter.h"

/*CPUÎÂ¶È¾ùÖµÂË²¨Æ÷*/
static FilterAverage<int> dfCPUTemp(13);

void Task_CPUInfoUpdate()
{
    CTRL.CPU.Temperature = (dfCPUTemp.getNext(Temp_ADC()) * 3300.0f / 4095.0f - 760.0f) / 2.5f + 25;
    CTRL.CPU.Usage = MainTask.GetCPU_Usage();
}
