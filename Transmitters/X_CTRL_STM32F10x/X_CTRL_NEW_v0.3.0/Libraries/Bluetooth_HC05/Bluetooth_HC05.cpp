#include "Bluetooth_HC05.h"

Bluetooth_HC05::Bluetooth_HC05(HardwareSerial &serialx, int16_t state_Pin, int16_t en_Pin, int16_t powerCtrl_Pin) 
			  : Serialx(&serialx)
{
    State_Pin = state_Pin;
    EN_Pin = en_Pin;
    PowerCtrl_Pin = powerCtrl_Pin;
}

void Bluetooth_HC05::Init(uint32_t baudRate)
{
    BaudRate = baudRate;
    Serialx->begin(BaudRate);
    Serialx->setTimeout(20);
    if(PowerCtrl_Pin != -1)pinMode(PowerCtrl_Pin, OUTPUT);
    if(State_Pin != -1)pinMode(State_Pin, INPUT);
    if(EN_Pin != -1)pinMode(EN_Pin, OUTPUT);
}

bool Bluetooth_HC05::Power(bool state)
{
    if(PowerCtrl_Pin != -1)
    {
        if(state)
        {
            digitalWrite(PowerCtrl_Pin, HIGH);
        }
        else
        {
            digitalWrite(PowerCtrl_Pin, LOW);
        }
        return true;
    }
    return false;
}

bool Bluetooth_HC05::AT_Enter()
{
    Serialx->begin(38400);
    digitalWrite(PowerCtrl_Pin, LOW);
    delay(100);
    digitalWrite(EN_Pin, HIGH);
    delay(100);
    digitalWrite(PowerCtrl_Pin, HIGH);
    delay(100);

    return (AT_Cmd("AT") != "ERROR" ? true : false);
}

void Bluetooth_HC05::AT_Exit()
{
    digitalWrite(PowerCtrl_Pin, LOW);
    digitalWrite(EN_Pin, LOW);
    delay(1500);
    digitalWrite(PowerCtrl_Pin, HIGH);
    Serialx->begin(BaudRate);
}

String Bluetooth_HC05::AT_Cmd(String Cmd, uint32_t TimeOut)
{
    uint32_t Exit_TimePoint = millis() + TimeOut;
    String AT_Return;
    Cmd.trim();

AT_Retry:
    Serialx->println(Cmd);

    while (millis() < Exit_TimePoint)
    {
        if (Serialx->available())
        {
            String AT_Return = Serialx->readString();
			if (AT_Return.startsWith("ERROR"))
			{
				delay(200);
				goto AT_Retry;
			}
            else
			{
				AT_Return.trim();
				return AT_Return;
			}
        }
    }
	
	Serialx->flush();
    return "ERROR";
}

bool Bluetooth_HC05::Set_Role(bool role, String name)
{
    if(AT_Cmd("AT+NAME=" + name) != "ERROR")//命名模块名
        return false;

    delay(200);

    if(AT_Cmd("AT+ROLE=" + String(role)) == "ERROR") //设置主从模式：0从机，1主机
        return false;

    return true;
}

bool Bluetooth_HC05::Set_Password(String passord)
{
    AT_Cmd("AT+CMODE=1");//蓝牙连接模式为任意地址连接模式
    delay(200);
    AT_Cmd("AT+PSWD=" + passord); //设置配对密码
    delay(200);
    AT_Cmd("AT+RMAAD");//清空配对列表
	
	return true;
}

bool Bluetooth_HC05::Set_BaudRate(uint32_t baudRate)
{
    if(AT_Cmd("AT+UART=" + String(baudRate) + ",0,0") != "ERROR") //设置波特率，停止位1，校验位无
        return true;
    else
        return false;
}

bool Bluetooth_HC05::Get_State()
{
	if(State_Pin != -1)
		return digitalRead(State_Pin);
	else
		return false;
}

String Bluetooth_HC05::Get_Password()
{
	String Passwrod = AT_Cmd("AT+PSWD?");
	
	if (Passwrod.startsWith("+PSWD:"))
		return Passwrod.substring(6);
	else
		return "NO PASSWORD";
}

uint32_t Bluetooth_HC05::Get_BaudRate()
{
	String BaudRate = AT_Cmd("AT+UART?");
	
	if(BaudRate != "ERROR")
		return BaudRate.toInt();
	else
		return 0;
}
