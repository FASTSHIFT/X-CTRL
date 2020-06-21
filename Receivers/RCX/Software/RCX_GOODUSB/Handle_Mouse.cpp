#include "FileGroup.h"
#include "ComPrivate.h"
#include "DigitalFilter.h"

//static HIDAbsMouse Mouse(HID);
static HIDMouse Mouse(HID);

#define MOUSE_AXIS_MAX 32767
#define MOUSE_USE_FILTER 0

typedef struct
{
    int16_t X;
    int16_t Y;
} Axis_t;

static void Setup()
{
  Com_SetDescription("Mouse");
  HID.begin(HID_MOUSE);
}

static int8_t WheelCheck()
{
  int8_t wheel = 0;

  if(abs(RCX::RxGetPackChannel(CH_JSR_X)) > (RCX_CHANNEL_DATA_MAX / 8))
  {
    wheel = map(RCX::RxGetPackChannel(CH_JSR_X), -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX, -4, 4);
  }

  return wheel;
}

static void Loop()
{
  Axis_t axis;
  float l_limit = (RCX::RxGetPackChannel(CH_LMT_L) / (float)RCX_CHANNEL_DATA_MAX) * 0.2f;
  float x = RCX::RxGetPackChannel(CH_JSL_X) * l_limit;
  float y = RCX::RxGetPackChannel(CH_JSR_Y) * l_limit;
  axis.X = __Map(x, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX, -MOUSE_AXIS_MAX, MOUSE_AXIS_MAX);
  axis.Y = __Map(y, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX, -MOUSE_AXIS_MAX, MOUSE_AXIS_MAX);

#if (MOUSE_USE_FILTER == 1)
  static FilterAverage<int32_t> dfMouseX(7);
  static FilterAverage<int32_t> dfMouseY(7);
  axis.X = dfMouseX.getNext(axis.X);
  axis.Y = dfMouseY.getNext(axis.Y);
#endif

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

  if (RCX::RxGetPackChannel(CH_JSL_Y) > (RCX_CHANNEL_DATA_MAX / 2))
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

  if (RCX::RxGetPackChannel(CH_JSL_Y) < -(RCX_CHANNEL_DATA_MAX / 2))
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
