#include "FileGroup.h"
#include "ComPrivate.h"
#include "DigitalFilter.h"

//static HIDAbsMouse Mouse(HID);
static HIDMouse Mouse(HID);

#define MOUSE_AXIS_MAX 32767

//const uint8_t reportDescription[] = {
//  HID_ABS_MOUSE_REPORT_DESCRIPTOR(HID_MOUSE_REPORT_ID)
//};

static void Setup()
{
//  HID.begin(reportDescription, sizeof(reportDescription));
  HID.begin(HID_MOUSE);
}

static int8_t WheelCheck()
{
  int8_t wheel = 0;

  if (RC_X.Right.X > (CtrlOutput_MaxValue / 2))
  {
    wheel = -1;
  }
  else if (RC_X.Right.X < -(CtrlOutput_MaxValue / 2))
  {
    wheel = 1;
  }

  return wheel;
}

static void Loop()
{
  Axis_t axis;
  float l_limit = (RC_X.KnobLimit.L / (float)CtrlOutput_MaxValue);
  float x = RC_X.Left.X * l_limit;
  float y = RC_X.Right.Y * l_limit;
  axis.X = __Map(x, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -MOUSE_AXIS_MAX, MOUSE_AXIS_MAX);
  axis.Y = __Map(y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -MOUSE_AXIS_MAX, MOUSE_AXIS_MAX);

//  static FilterAverage<int32_t> dfMouseX(7);
//  static FilterAverage<int32_t> dfMouseY(7);
//  axis.X = dfMouseX.getNext(axis.X);
//  axis.Y = dfMouseY.getNext(axis.Y);

  int8_t wheel = 0;

  __IntervalExecute(wheel = WheelCheck(), 50);
  __IntervalExecute(
    Mouse.move(
      axis.X,
      axis.Y,
      wheel
    ),
    20
  );

  if (RC_X.Left.Y > (CtrlOutput_MaxValue / 2))
  {
    if (!Mouse.isPressed(MOUSE_LEFT))
    {
      Mouse.press(MOUSE_LEFT);
    }
  }
  else
  {
    if (Mouse.isPressed(MOUSE_LEFT))
    {
      Mouse.release(MOUSE_LEFT);
    }
  }

  if (RC_X.Left.Y < -(CtrlOutput_MaxValue / 2))
  {
    if (!Mouse.isPressed(MOUSE_RIGHT))
    {
      Mouse.press(MOUSE_RIGHT);
    }
  }
  else
  {
    if (Mouse.isPressed(MOUSE_RIGHT))
    {
      Mouse.release(MOUSE_RIGHT);
    }
  }
}

void Mouse_Reg(uint8_t id)
{
  Sim_RegFunc(id, Setup, Loop);
}
