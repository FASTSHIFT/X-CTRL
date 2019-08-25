#include "FileGroup.h"
#include "ComPrivate.h"

#define DEBUG(...)

extern Protocol_Handshake_t HandshakePack_Master;
extern Protocol_Handshake_t HandshakePack_Slave[SlaveNum_MAX];

void HandshakeToSlave(uint8_t id)
{
  HandshakeRun(HandshakeState_Prepare);

  uint8_t slavenum = 0;
  DEBUG("\r\nSearching...");
  while (1)
  {
    __IntervalExecute(togglePin(InfoLED_Pin), 2000);
    uint8_t n = HandshakeRun(HandshakeState_Search);
    if (n)
    {
      for (uint8_t i = 0; i < SlaveNum_MAX; i++)
      {
        if (HandshakePack_Slave[i].ID == id)
        {
          slavenum = i;
          tone(Buzz_Pin, 1000, 100);
          goto ConnectSlave;
        }
      }
    }
  }

ConnectSlave:
  uint32_t timeout = millis() + 3000;
  while (HandshakeRun(HandshakeState_ReqConnect, slavenum, CMD_AttachConnect) != CMD_AgreeConnect && millis() < timeout)
  {
    DEBUG("\r\nTry connect to Slave[%d]...", slavenum);
    //    tone(Buzz_Pin, 1000, 10);
    //    delay(20);
  }
  tone(Buzz_Pin, 2000, 100);

  HandshakeRun(HandshakeState_Connected);
  DEBUG("\r\nConnect Slave[%d] success!!", slavenum);
}
