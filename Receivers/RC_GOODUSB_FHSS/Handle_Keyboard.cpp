#include "FileGroup.h"
#include "ComPrivate.h"
#include "FifoQueue.h"

static HIDKeyboard Keyboard(HID);

static FifoQueue<char> KeyboardStr(100);

static void EnterCMD()
{
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.release(KEY_LEFT_GUI);
  Keyboard.release('r');
  delay(100);

  Keyboard.write(KEY_CAPS_LOCK);

  delay(100);

  Keyboard.print("cmd\r\n");

  Keyboard.releaseAll();
}

static void NRF_Keyboard_IRQHandler()
{
  togglePin(LED_Pin);
  NRF_RxBuff[0] = '\0';
  nrf.Recv(NRF_RxBuff);
  uint8_t i = 0;
  do {
    KeyboardStr.write(NRF_RxBuff[i]);
    i++;
  } while (NRF_RxBuff[i] != '\0');
}

static void Setup()
{
  UseMtm = false;
  //USBHID_begin_with_serial(HID_KEYBOARD);
  HID.begin(HID_KEYBOARD);
  delay(1000);
  Keyboard.begin();
  Keyboard.releaseAll();
}

static void Loop()
{
  if (nrf.GetStatus() & nrf.RX_DR)
  {
    NRF_Keyboard_IRQHandler();
  }

  if (KeyboardStr.available())
  {
    togglePin(LED_Pin);
    char c = KeyboardStr.read();
    if (c == '~')
      EnterCMD();
    else
      Keyboard.write(c);
  }
}

void Keyboard_Reg(uint8_t id)
{
  Sim_RegFunc(id, Setup, Loop);
}

/********* KEY MAP DEFINE **********
     Key       Hexadecimal  Decimal
  KEY_LEFT_CTRL     0x80       128
  KEY_LEFT_SHIFT    0x81       129
  KEY_LEFT_ALT      0x82       130
  KEY_LEFT_GUI      0x83       131
  KEY_RIGHT_CTRL    0x84       132
  KEY_RIGHT_SHIFT   0x85       133
  KEY_RIGHT_ALT     0x86       134
  KEY_RIGHT_GUI     0x87       135
  KEY_UP_ARROW      0xDA       218
  KEY_DOWN_ARROW    0xD9       217
  KEY_LEFT_ARROW    0xD8       216
  KEY_RIGHT_ARROW   0xD7       215
  KEY_BACKSPACE     0xB2       178
  KEY_TAB           0xB3       179
  KEY_RETURN        0xB0       176
  KEY_ESC           0xB1       177
  KEY_INSERT        0xD1       209
  KEY_DELETE        0xD4       212
  KEY_PAGE_UP       0xD3       211
  KEY_PAGE_DOWN     0xD6       214
  KEY_HOME          0xD2       210
  KEY_END           0xD5       213
  KEY_CAPS_LOCK     0xC1       193
  KEY_F1            0xC2       194
  KEY_F2            0xC3       195
  KEY_F3            0xC4       196
  KEY_F4            0xC5       197
  KEY_F5            0xC6       198
  KEY_F6            0xC7       199
  KEY_F7            0xC8       200
  KEY_F8            0xC9       201
  KEY_F9            0xCA       202
  KEY_F10           0xCB       203
  KEY_F11           0xCC       204
  KEY_F12           0xCD       205
*******************************/
