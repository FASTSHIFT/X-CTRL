//Designed By _VIFEXTech

//Finish in 2017.7.16 V_1.0 支持多通道双向通信，与"RC_Recver"配合使用
//Update 7.19 V_1.1 移植菜单，数据掉电保存
//Update 7.20 V_1.2 新增新的遥控模式
//Update 7.26 V_1.3 增加单手遥控模式，NRF24L01 2.4G 遥控模块支持
//Update 7.29 V_1.4 增加两路舵机
//Update 8.16 V_1.5 NRF24L01打库
//Update 8.17 V_1.6 取消共用体
//Update 8.19 V_1.7 新增TANK Mode，支持调整波特率
//Update 8.27 V_1.8 支持MPU6050重力感应
//Update 10.2 V_1.9 支持模拟无线游戏手柄,与"RC_Mouse"配合使用(for Arduino Leonardo)
//Update 10.4 V_2.0 支持重力感应模拟鼠标，加入新的音效，新的字符出现效果
//Update 11.4 V_2.1 优化变量表示与程序结构
//Update 11.7 V_2.2 升级为(2+14+2)通信协议，旧通信协议已舍弃，增加加操作音与静音模式(在在开机时按住OK键)
//Update 11.12 V_2.3 为蜘蛛写了专用界面
//Update 11.15 V_2.4 修复了在菜单中也能控制蜘蛛动作的BUG
//Update 11.26 V_2.5 新增HC-05自动配置功能，将按键标志位转为数组形式
//Update 11.27 V_2.6 支持在菜单中读取和修改蓝牙密码
//Update 12.5 V_2.7 更新NRF库,支持配置NRF地址
//Update 2018.1.31 V_2.8 修复波特率变量溢出的BUG
//Update 2.6 V_2.9 支持调整是否编译MPU6050相关代码
//Update 2.7 V_3.0 加入0.96 OLED(SSD1306) I2C支持
//Update 2.9 V_3.1 优化接收程序
//Update 2.10 V_3.2 将长字符串放入Flash中以节省SRAM占用，菜单代码分块
//Update 2.11 V_3.3 加入结构体，优化舵机控制,使用与发送程序一样的接收协议，为Crawler写了新界面
//Update 2.13 V_3.4 加入Crawler无线图传系统电池电压监控，加入舵机常用角度快速控制，使用图形表示控制模式
//Update 5.13 V_3.5 更新MPU6050读取函数，不再使用卡尔曼滤波，减少Flash占用
//Update 5.19 V_3.6 支持用重力感应控制CAR、CRAWLER
//Update 6.8  V_3.7 增加数据包标记
//Update 6.10 V_3.8 整理代码
//Update 6.15 V_3.9 优化菜单，优化波特率调整
//Update 6.16 V_4.0 支持NRF半双工数据回传

#include "FileGroup.h"

#ifdef USE_MPU6050
MPU6050 mpu;
int16_t MPU_Pitch, MPU_Roll, MPU_Null;
#endif

SSD1306_I2C oled;
LiquidCrystal_I2C lcd(0x3F, 16, 2);//0x3F
NRF nrf(D11, D12, D13, D4, D2); //MOSI,MISO,SCK,CE,CSN
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

CtrlParamter_t JS_L;
CtrlParamter_t JS_R;
CtrlParamter_t ServoPos;

int L_PWM, R_PWM, B_PWM;
int L_CRE, R_CRE;

//************* GPIO **************//
//Others
uint8_t KEY_UP_Pin; //D12
uint8_t KEY_BACK_Pin; //D11
uint8_t KEY_DOWN_Pin; //D9
uint8_t KEY_R_Pin; //D6
uint8_t KEY_L_Pin; //D5

//*************Value**************//

//******从机******//
float Slave_Temp, Slave_BattVol;
float CR_kpa, CR_alt, CR_roll, CR_pitch, CR_FPV_Vol;
float Car_Kph, Car_RPS;
//***************//

uint8_t Key[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint8_t CTRL_State = Off;//启动控制

uint8_t CTRL_Module = 1;//模块选择 0:UART 1:NRF24L01

int CTRL_Object;//操纵对象

uint8_t MenuState = OPT;//0:显示信息 1:进入设置菜单

const uint32_t UART_Baud[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200}; //常用波特率
uint8_t UART_Baud_Select = 10;//通信波特率选择

uint32_t Recv_TimePoint;//接收时间点
uint32_t Tran_TimePoint;//发送时间点
uint16_t SendFreq;//数据包发送频率
uint8_t CtrlMode;//操纵模式 1:左右独立控制 2:左油门，右转向 3:左油门转向，右舵机

uint8_t MPU_Switch = Off;//加速度计
uint8_t BackLight_Switch = On;//背光
uint8_t RingLast;
uint8_t Silent_Mode = Off;

//********************* LCD自定义字符 ********************//
uint8_t ANT[] = {0x00, 0x1F, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
uint8_t ANT_full[] = {0x00, 0x01, 0x01, 0x05, 0x05, 0x15, 0x15, 0x15};
uint8_t Tempdc[] = {0x1C, 0x14, 0x1C, 0x07, 0x08, 0x08, 0x08, 0x07};
uint8_t Arrow_UpDown[] = {0x04, 0x0E, 0x15, 0x04, 0x04, 0x15, 0x0E, 0x04};
uint8_t Arrow_Left[] = {0x00, 0x00, 0x04, 0x08, 0x1F, 0x08, 0x04, 0x00};
uint8_t Arrow_Right[] = {0x00, 0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00};

void setup()
{
	Wire.begin();
    lcd.init();
    oled.init();

    lcd.createChar(0, ANT);
    lcd.createChar(1, ANT_full);
    lcd.createChar(2, Tempdc);
    lcd.createChar(3, Arrow_UpDown);
    lcd.createChar(4, Arrow_Left);
    lcd.createChar(5, Arrow_Right);

    pinMode(KEY_OK_Pin, INPUT_PULLUP);
	pinMode(BUZZ_Pin, OUTPUT);

    if (!digitalRead(KEY_OK_Pin))
	{
		delay(10);
		if (!digitalRead(KEY_OK_Pin))
			Silent_Mode = On;//静音模式启动
	}

    lcd.backlight();
    LCD_DynamicPrint(5, 0, (char*)"RC-CTRL", 5);//LOGO
    LCD_DynamicPrint(3, 1, (char*)"_VIFEXTech", 5);
    oled.setCursor(5, 0), oled.print("RC-CTRL");
    oled.setCursor(3, 1), oled.print("_VIFEXTech");
    delay(100);
    BuzzRing(4);

#ifdef USE_MPU6050
    mpu.initialize();
#endif

    delay(500);

    lcd.clear();
    oled.clear();
    oled.setCacheMode(On);
    DataStorageHandle(ReadData);

    if (UART_Baud_Select > 10)UART_Baud_Select = UART_Baud_Select_Default;
    Serial.begin(UART_Baud[UART_Baud_Select]);

    if (CTRL_Module == Module_NRF)
    {
        KEY_UP_Pin = D6;
        KEY_BACK_Pin = D9;
        KEY_DOWN_Pin = D5;
        KEY_R_Pin = D6;
        KEY_L_Pin = D5;

        if (CTRL_Object == SPIDER)nrf.SetAddress(12, 34, 56, 78, 90);
        else nrf.SetAddress(55, 44, 33, 22, 11);
        //nrf.init(TX_MODE, TX_buff, sizeof(TX_buff));//MODE, TX_buff, TX_width
        nrf.init(TXRX_MODE, TX_buff, sizeof(TX_buff), sizeof(RX_buff));
        uint8_t NRF_status = nrf.Status();
        lcd.setCursor(2, 0);
        lcd.print(F("NRF:"));
        lcd.setCursor(7, 0);
        if (NRF_status != 0 || NRF_status != 0xFF)
        {
            lcd.print("Online");
            lcd.setCursor(1, 1);
            for (uint8_t i = 0; i < 5; i++)
            {
                lcd.print(nrf.GetAddress(i));
                lcd.print(' ');
            }
            BuzzRing(2);
        }
        else
        {
            lcd.print("Offline");
            BuzzRing(1);
        }
        delay(1000);
        lcd.clear();
    }
    else
    {
        KEY_UP_Pin = D12;
        KEY_BACK_Pin = D11;
        KEY_DOWN_Pin = D9;
        KEY_R_Pin = D6;
        KEY_L_Pin = D5;
    }

    pinMode(KEY_UP_Pin, INPUT_PULLUP);
    pinMode(KEY_BACK_Pin, INPUT_PULLUP);
    pinMode(KEY_OK_Pin, INPUT_PULLUP);
    pinMode(KEY_DOWN_Pin, INPUT_PULLUP);
    pinMode(KEY_JSR_Pin, INPUT_PULLUP);
    pinMode(KEY_JSL_Pin, INPUT_PULLUP);
    pinMode(KEY_R_Pin, INPUT_PULLUP);
    pinMode(KEY_L_Pin, INPUT_PULLUP);
    if (SendFreq == 0)SendFreq = SendFreq_Default;
    if (CtrlMode == 0)CtrlMode = CtrlMode_Default;
    MsTimer2_set(SendFreq, SendData);
}

void loop()
{
    if (CTRL_Module == Module_UART && Serial.available() >= RX_DataLength)RX_DataLoad();
    else if (CTRL_Module == Module_NRF)
    {
        NRF_CheckConnection();
        RX_Transform();
    }
    Read_Joystick();
    Read_Keys();

#ifdef USE_MPU6050
    if (MPU_Switch == On)Read_MPU6050();
#endif

    LoadCtrlMode(CtrlMode);
    if (MenuState == INF) TX_DataLoad();
    MenuRun();
}

