#include "Basic/FileGroup.h"
#include "BSP.h"
#include "Bluetooth_HC05/Bluetooth_HC05.h"

/*实例化HC05蓝牙对象*/
static Bluetooth_HC05 hc05(&BT_Serial, BT_State_Pin, BT_EN_Pin, BT_Power_Pin);

/*蓝牙可选波特率*/
static const uint32_t UseBaudRate[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200};

/*蓝牙可选波特率个数*/
#define UseBaudRate_Size __Sizeof(UseBaudRate)

void Bluetooth_Init()
{
    hc05.Init(UseBaudRate[UseBaudRate_Size - 1]);
    hc05.Power(CTRL.Bluetooth.Enable);
}

void Bluetooth_UpdatePowerState()
{
    hc05.Power(CTRL.Bluetooth.Enable);
}
