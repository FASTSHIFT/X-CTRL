#include "FileGroup.h"
#include "ComPrivate.h"

USBXBox360 XBox360;

#define XBox_MAX 32767
#define HeadCode_XBox (uint16_t)0x0036

//#define XBOX_RACE_MODE

#pragma pack (1)
typedef struct {
    uint16_t HeadCode;
    uint8_t LED_Pattern[4];
    uint8_t Motor[2];
} Protocol_XBox360_Passback_t;
#pragma pack ()

Protocol_XBox360_Passback_t xbox;

static void LEDCallback(uint8_t pattern)
{

}

static void RumbleCallback(uint8_t left, uint8_t right)
{
  xbox.HeadCode = HeadCode_XBox;
  xbox.Motor[0] = left;
  xbox.Motor[1] = right;
  __TypeExplain(Protocol_XBox360_Passback_t, NRF_TxBuff) = xbox;
  togglePin(LED_Pin);
}

static void JoystickCtrl()
{
#ifdef XBOX_RACE_MODE
  static uint8_t sl, sr;
  if (RC_X.Left.Y >= 0)
  {
    sl = 0;
    sr = map(RC_X.Left.Y, 0, CtrlOutput_MaxValue, 0, 255);
  }
  else
  {
    sl = map(-RC_X.Left.Y, 0, CtrlOutput_MaxValue, 0, 255);
    sr = 0;
  }
  int16_t xr = map(RC_X.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -XBox_MAX, XBox_MAX);
  int16_t yr = map(RC_X.Right.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -XBox_MAX, XBox_MAX);
  XBox360.sliderLeft(sl);
  XBox360.sliderRight(sr);
  XBox360.position(xr, yr);
#else
  int16_t x = map(RC_X.Left.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -XBox_MAX, XBox_MAX);
  int16_t y = map(RC_X.Left.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -XBox_MAX, XBox_MAX);
  int16_t xr = map(RC_X.Right.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -XBox_MAX, XBox_MAX);
  int16_t yr = map(RC_X.Right.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, -XBox_MAX, XBox_MAX);
  XBox360.position(x, y);
  XBox360.positionRight(xr, yr);
#endif
}

static void TriggerCtrl()
{
}


static void ButtonCtrl()
{
  typedef enum {
    DPAD_UP = 1,
    DPAD_DOWN = 2,
    DPAD_LEFT = 3,
    DPAD_RIGHT = 4,
    BUTTON_A = 13,//->1
    BUTTON_B = 14,//->2
    BUTTON_X = 15,//->3
    BUTTON_Y = 16,//->4
    BUTTON_LB = 9,//->5
    BUTTON_RB = 10,//->6
    BUTTON_VIEW = 11,//->7
    BUTTON_MENU = 12,//->8
    BUTTON_LTC = 8,//->9
    BUTTON_RTC = 7,//->10
  } XBOX_Button_Type;
  static uint8_t KeyLast;
  if (RC_X.Key != KeyLast)
  {
    KeyLast = RC_X.Key;
    XBox360.button(BUTTON_Y, RC_X.Key & BT_UP);
    XBox360.button(BUTTON_X, RC_X.Key & BT_LEFT);
    XBox360.button(BUTTON_A, RC_X.Key & BT_DOWN);
    XBox360.button(BUTTON_B, RC_X.Key & BT_RIGHT);
    XBox360.button(BUTTON_LB, RC_X.Key & BT_L2);
    XBox360.button(BUTTON_RB, RC_X.Key & BT_R2);
    XBox360.button(BUTTON_LTC, RC_X.Key & BT_L1);
    XBox360.button(BUTTON_RTC, RC_X.Key & BT_R1);
  }
  //    for (uint8_t i = 0; i < 10; i++)
  //    {
  //      XBox360.button(BUTTON_A, true);
  //      delay(100);
  //      XBox360.button(BUTTON_A, false);
  //      delay(100);
  //    }
  //
  //    for (uint8_t i = 1; i <= 16; i++)
  //    {
  //      XBox360.button(i, true);
  //      delay(1000);
  //      XBox360.button(i, false);
  //      delay(1000);
  //    }
}

static void DpadCtrl()
{
}

static void Setup()
{
  XBox360.begin();
  XBox360.setLEDCallback(LEDCallback);
  XBox360.setRumbleCallback(RumbleCallback);
  XBox360.setManualReportMode(true);
}

static void Loop()
{
  if (ConnectState.IsConnect && ConnectState.IsCorrect)
  {
    JoystickCtrl();
    TriggerCtrl();
    ButtonCtrl();
    DpadCtrl();
    XBox360.send();
  }
  else
  {
  }
}

void XBox360_Reg(uint8_t id)
{
  Sim_RegFunc(id, Setup, Loop);
}
