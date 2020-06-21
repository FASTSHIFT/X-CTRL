#include "FileGroup.h"
#include "ComPrivate.h"

void Task_InfoLED()
{
  if (RCX::RxGetConnected())
  {
    digitalWrite_HIGH(LED_Pin);
  }
  else if (RCX::RxGetPackErrorCode())
  {
    mtm.TaskList[TP_InfoLED].IntervalTime = 100;
    togglePin(LED_Pin);
  }
  else
  {
    mtm.TaskList[TP_InfoLED].IntervalTime = 200;
    togglePin(LED_Pin);
  }
}
