#include "FileGroup.h"

#define BT_MASTER_NAME "X-CTRL_Master"
#define BT_SLAVE_NAME "X-CTRL_Slave"

char BT_password[5]= "0000";
String AT_Return, PSWD;
uint8_t AT_CMD(String cmd);

void BT_Setup(void)
{
  uint8_t Role = 1, PSWB_b = 0, Role_b = 0;
	int8_t pos = 0, pos_b = 1;
  
	BlueTooth.begin(38400);
  BlueTooth.setTimeout(10);

	
	digitalWrite(BT_PowerCtrl_Pin, LOW);
	delay(1000);
	digitalWrite(BT_EN_Pin, HIGH);
	delay(1000);
	digitalWrite(BT_PowerCtrl_Pin, HIGH);
	delay(1000);

  if (AT_CMD("AT"))
  {
    AT_CMD("AT+PSWD?");

    while (pos <= 4)
    {
      Read_Keys();
      if (pos_b != pos || PSWB_b != BT_password[pos] || Role_b != Role)
      {
        
        pos_b = pos;
        PSWB_b = BT_password[pos];
        Role_b = Role;
      }

      if (Key[k_up] && pos <= 3)Key[k_up] = 0, BT_password[pos]++;
      if (Key[k_down] && pos >= 0)Key[k_down] = 0, BT_password[pos]--;
			BT_password[pos] = constrain(BT_password[pos],'0','9');

      if (Key[k_ok])Key[k_ok] = 0, pos++;
      if (Key[k_back] && pos > 0)Key[k_back] = 0, pos--;

      if (pos == 4 && (Key[k_up] || Key[k_down]))
      {
        Key[k_up] = Key[k_down] = 0;
        Role = !Role;
      }
    }

    delay(200);
    if (Role)AT_CMD("AT+NAME=" + String(BT_MASTER_NAME)); //命名模块名
    else AT_CMD("AT+NAME=" + String(BT_SLAVE_NAME));
    delay(200);
    AT_CMD("AT+ROLE=" + String(Role)); //设置主从模式：0从机，1主机
    delay(200);
    AT_CMD("AT+CMODE=1");//蓝牙连接模式为任意地址连接模式
    delay(200);
    PSWD = String(BT_password);
    PSWD = PSWD.substring(0, 4);
    AT_CMD("AT+PSWD=" + PSWD); //设置配对密码，如1234
    delay(200);
    AT_CMD("AT+UART=" + String(UART_Baud[CTRL.Baud_Select]) + ",0,0"); //设置波特率，停止位1，校验位无
    delay(200);
    AT_CMD("AT+RMAAD");//清空配对列表
    //    delay(200);
    //    AT_CMD("AT+RESET");//退出AT模式

		digitalWrite(BT_PowerCtrl_Pin, LOW);
		digitalWrite(BT_EN_Pin, LOW);
		delay(1500);
		digitalWrite(BT_PowerCtrl_Pin, HIGH);
  }
  else
  {
		digitalWrite(BT_PowerCtrl_Pin, HIGH);
		digitalWrite(BT_EN_Pin, LOW);
    delay(1500);
  }
	
  BlueTooth.begin(UART_Baud[CTRL.Baud_Select]);
}

uint8_t AT_CMD(String cmd)
{
  unsigned long TimeOut = millis() + 3000;
  cmd.trim();
retry:
  BlueTooth.println(cmd);

  while (millis() < TimeOut)
  {
    if (BlueTooth.available())
    {
      AT_Return = BlueTooth.readString();
      AT_Return.trim();
      delay(200);
      if (AT_Return.startsWith("+PSWD:"))
      {
        PSWD = AT_Return.substring(6, 10);
        PSWD.toCharArray(BT_password, 5);
      }
      break;
    }
  }
  if (millis() >= TimeOut)
  {
    BlueTooth.flush();
    return 0;
  }
  else if (AT_Return.startsWith("ERROR"))
  {
    goto retry;
  }
  else return 1;
}

