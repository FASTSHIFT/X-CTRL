#include "FileGroup.h"

char BT_password[5];
String AT_Return, PSWD;

void BT_Setup()
{
    uint8_t Role = 1, pos = 0, pos_b = 1, PSWB_b = 0, Role_b = 0;
    Serial.begin(38400);
    Serial.setTimeout(10);

    lcd.clear();

    if (AT_CMD("AT"))
    {
        AT_CMD("AT+PSWD?");

        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(F("Set Password:"));

        while (pos <= 4)
        {
            Read_Keys();
            if (pos_b != pos || PSWB_b != BT_password[pos] || Role_b != Role)
            {
                lcd.setCursor(1, 1);
                lcd.print(BT_password);
                lcd.setCursor(5, 1);
                if (Role)lcd.print(F(" Master"));
                else lcd.print(F(" Slave "));
                lcd.setCursor(pos + 1, 1);
                lcd.blink();
                pos_b = pos;
                PSWB_b = BT_password[pos];
                Role_b = Role;
            }

            if (Key[k_up] && pos <= 3)Key[k_up] = 0, BT_password[pos]++;
            if (Key[k_down] && pos > 0)Key[k_down] = 0, BT_password[pos]--;
            if (BT_password[pos] < '0')BT_password[pos] = '0';
            if (BT_password[pos] > '9')BT_password[pos] = '9';

            if (Key[k_ok])Key[k_ok] = 0, pos++;
            if (Key[k_back] && pos > 0)Key[k_back] = 0, pos--;

            if (pos == 4 && (Key[k_up] || Key[k_down]))
            {
                Key[k_up] = Key[k_down] = 0;
                Role = !Role;
            }
        }
        lcd.noBlink();

        delay(200);
        //Role = digitalRead(KEY_OK_Pin);
        if (Role)AT_CMD("AT+NAME=RC_Master"); //命名模块名
        else AT_CMD("AT+NAME=RC_Slave");
        delay(200);
        AT_CMD("AT+ROLE=" + String(Role)); //设置主从模式：0从机，1主机
        delay(200);
        AT_CMD("AT+CMODE=1");//蓝牙连接模式为任意地址连接模式
        delay(200);
        PSWD = String(BT_password);
        PSWD = PSWD.substring(0, 4);
        AT_CMD("AT+PSWD=" + PSWD); //设置配对密码，如1234
        delay(200);
        AT_CMD("AT+UART=" + String(UART_Baud[UART_Baud_Select]) + ",0,0"); //设置波特率，停止位1，校验位无
        delay(200);
        AT_CMD("AT+RMAAD");//清空配对列表
        //    delay(200);
        //    AT_CMD("AT+RESET");//退出AT模式

        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(F("Config Done"));
        delay(1500);
        lcd.clear();
    }
    else
    {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(F("No Respon"));
        delay(1500);
        lcd.clear();
    }

    Serial.begin(UART_Baud[UART_Baud_Select]);
}

uint8_t AT_CMD(String cmd)
{
    uint32_t TimeOut = millis() + 2000;
    cmd.trim();
retry:
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(cmd);
    Serial.println(cmd);

    lcd.setCursor(1, 1);
    while (millis() < TimeOut)
    {
        if (Serial.available())
        {
            AT_Return = Serial.readString();
            AT_Return.trim();
            lcd.print(AT_Return);
            delay(200);
            if (AT_Return.startsWith("+PSWD:"))
            {
                PSWD = AT_Return.substring(6, 10);
                lcd.clear();
                PSWD.toCharArray(BT_password, 5);
//        lcd.print(PSWD);
//        lcd.setCursor(0, 1);
//        lcd.print(BT_password);
//        delay(2000);
            }
            break;
        }
    }
    if (millis() >= TimeOut)
    {
        Serial.flush();
        lcd.print(F("NO Reply!"));
        return 0;
    }
    else if (AT_Return.startsWith("ERROR"))
    {
        goto retry;
    }
    else return 1;
}

