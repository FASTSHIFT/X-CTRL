#include "HMI_Ctrl.h"
#include <stdarg.h>

using namespace HMI;

Basic::Basic(HardwareSerial *Serialx)
{
    Hmi = Serialx;
}

void Basic::End()
{
    Hmi->write(0xFF);
    Hmi->write(0xFF);
    Hmi->write(0xFF);
}

Widget::Widget(HardwareSerial *serialx, String widgetstr) : Basic(serialx)
{
    WidgetStr = widgetstr;
}

void Widget::SetValue(const char *__restrict __format, ...)
{
    Hmi->print(WidgetStr);
    va_list args;
    va_start(args, __format);
    Hmi->printf(__format, args);
    va_end(args);
    End();
}
