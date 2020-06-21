#include "FileGroup.h"
#include "ComPrivate.h"

USBXBox360 XBox360;

static const int XBox_MAX = 32767;

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

static int16_t BattVoltage = 5000;
static int16_t BattLevel = RCX_CHANNEL_DATA_MAX;
static int16_t MotorLeft;
static int16_t MotorRight;

static void LEDCallback(uint8_t pattern)
{

}

static void RumbleCallback(uint8_t left, uint8_t right)
{
  MotorLeft = map(left, 0, 255, 0, RCX_CHANNEL_DATA_MAX);
  MotorRight = map(right, 0, 255, 0, RCX_CHANNEL_DATA_MAX);
  togglePin(LED_Pin);
}

static int16_t channelMap(int16_t chVal)
{
  return map(chVal, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX, -XBox_MAX, XBox_MAX);
}

static void XBox360_positionSlider(int16_t z)
{
  uint8_t sl, sr;
  if (z >= 0)
  {
    sl = 0;
    sr = map(z, 0, XBox_MAX, 0, 0xFF);
  }
  else
  {
    sl = map(-z, 0, XBox_MAX, 0, 0xFF);
    sr = 0;
  }
  XBox360.sliderLeft(sl);
  XBox360.sliderRight(sr);
}

static void JoystickCtrl()
{
  int16_t x, y;
  int16_t xr, yr;
  int16_t z;

  x = channelMap(RCX::RxGetPackChannel(CH_JSL_X));
  y = channelMap(RCX::RxGetPackChannel(CH_JSL_Y));
  xr = channelMap(RCX::RxGetPackChannel(CH_JSR_X));
  yr = channelMap(RCX::RxGetPackChannel(CH_JSR_Y));
  z = channelMap(RCX::RxGetPackChannel(2));

  XBox360.position(x, y);
  XBox360.positionRight(xr, yr);
  XBox360_positionSlider(z);
}

static void ButtonTest()
{
  for (uint8_t i = 0; i < 10; i++)
  {
    XBox360.button(BUTTON_A, true);
    delay(100);
    XBox360.button(BUTTON_A, false);
    delay(100);
  }

  for (uint8_t i = 1; i <= 16; i++)
  {
    XBox360.button(i, true);
    delay(1000);
    XBox360.button(i, false);
    delay(1000);
  }
}

static void ButtonCtrl()
{
  uint8_t Key = RCX::RxGetPackKey();

  XBox360.button(BUTTON_Y, Key & BT_UP);
  XBox360.button(BUTTON_X, Key & BT_LEFT);
  XBox360.button(BUTTON_A, Key & BT_DOWN);
  XBox360.button(BUTTON_B, Key & BT_RIGHT);
  XBox360.button(BUTTON_LB, Key & BT_L2);
  XBox360.button(BUTTON_RB, Key & BT_R2);
  XBox360.button(BUTTON_LTC, Key & BT_L1);
  XBox360.button(BUTTON_RTC, Key & BT_R1);
}

static void Setup()
{
  Com_SetDescription("XBox360");
  XBox360.begin();
  XBox360.setLEDCallback(LEDCallback);
  XBox360.setRumbleCallback(RumbleCallback);
  XBox360.setManualReportMode(true);
}

static void Com_PassbackInit()
{
  RCX::ChannelSetAttachEnable(true);
  RCX::ChannelSetAttach(0, &BattVoltage);
  RCX::ChannelSetAttach(1, &BattLevel);
  RCX::ChannelSetAttach(2, &MotorLeft);
  RCX::ChannelSetAttach(3, &MotorRight);
  RCX::TxSetObjectType(RCX_TYPE);
  RCX::TxSetObjectID(RCX::Handshake::GetSlaveID());
}

static void Loop()
{
  __ExecuteOnce(Com_PassbackInit());

  if (!RCX::RxGetConnected())
    return;

  JoystickCtrl();
  ButtonCtrl();
  XBox360.send();
}

void XBox360_Reg(uint8_t id)
{
  Sim_RegFunc(id, Setup, Loop);
}
