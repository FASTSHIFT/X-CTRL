#ifndef __HMI_CTRL_H
#define __HMI_CTRL_H

#include "HardwareSerial.h"

namespace HMI {

class Basic {
public:
    Basic(HardwareSerial *Serialx);
    void End();

    HardwareSerial *Hmi;
private:
};

class Widget : public Basic{
public:
    Widget(HardwareSerial *serialx, String widgetstr);
    void SetValue(const char * format, ...);
private:
    String WidgetStr;
};

};

#endif
