#include "FileGroup.h"
#include "GUI_Private.h"
PageManager page(PAGE_MAX, EVENT_MAX);

void Init_Display()
{
    HMI_SERIAL.begin(115200);
    HMI_SERIAL.setTimeout(20);
}

void Init_Pages()
{
}

void Init_GUI()
{
    Init_Pages();
}

void Thread_GUI()
{
    page.Running();
}
