#include "FileGroup.h"
#include "ComPrivate.h"

#define BattDet_Pin CH2_Pin
#define __ADCToVoltage(x) (x/4095.0*3.3*6.0)

void Init_Sensor()
{
    pinMode(BattDet_Pin, INPUT_ANALOG);
}

void Task_SensorUpdate()
{
    static uint8_t ReportState = 0;
    ReportState ++;
    ReportState %= 2;
    switch(ReportState)
    {
    case 0:
        PassbackData.X = 45;
        PassbackData.Y = 30;
        PassbackData.TextSize = 1;
        PassbackData.TextColor = 0xF800;
        PassbackData.BackColor = 0x0000;
        strcpy(PassbackData.Text, Slave_Description);
        break;
    case 1:
        PassbackData.X = 13;
        PassbackData.Y = 40;
        PassbackData.TextSize = 2;
        PassbackData.TextColor = 0xFFFF;
        PassbackData.BackColor = 0x0000;
        sprintf(PassbackData.Text, "%04.1fV", __ADCToVoltage(analogRead(BattDet_Pin)));
        break;
    }
}
