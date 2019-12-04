#include "FileGroup.h"
#include "ComPrivate.h"
#include "LuaGroup.h"

#define Sending_TimeOut 2000
#define SendStrLen (sizeof(NRF_TxBuff)-1)

bool Keyboard_Print(char* str)
{
    bool status = true;

    State_RF = 0;
    uint32_t timeOut;
    uint16_t strLength = strlen(str);
    uint16_t cnt = 0, sum = strLength / sizeof(NRF_TxBuff) + 1;
    
    memset(NRF_TxBuff, 0, sizeof(NRF_TxBuff));
    
    nrf.TX_Mode(NRF_TxBuff);

    while(cnt < sum)
    {
        timeOut = millis();

        memcpy(NRF_TxBuff, str, SendStrLen);
        Lua_SERIAL.printf("Send >> %s\r\n",NRF_TxBuff);

        str += SendStrLen;
        cnt++;
   
        while(!(nrf.Tran(NRF_TxBuff) & nrf.TX_DS))
        {
            if(millis() - timeOut > Sending_TimeOut)
            {
                status = false;
                Lua_SERIAL.print(" (Failed)\r\n");
                goto Exit;
            }
        }
        Lua_SERIAL.print(" (Done)\r\n");
    }
Exit:

    memset(NRF_TxBuff, 0, sizeof(NRF_TxBuff));

    return status;
}
