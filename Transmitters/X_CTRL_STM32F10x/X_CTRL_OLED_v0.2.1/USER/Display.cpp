#include "FileGroup.h"

SSD1306_I2C oled;

//纵向取模 8x8 字节倒序
uint8_t Triangle[] = 				{0x00, 0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00};
uint8_t Triangle_UD[] = 		{0x00, 0x00, 0x24, 0x66, 0xE7, 0x66, 0x24, 0x00};
uint8_t ANT[] = 						{0x02, 0x06, 0x0A, 0xF2, 0xF2, 0x0A, 0x06, 0x02};
uint8_t ANT_full[] = 				{0x00, 0xF0, 0x00, 0xFC, 0x00, 0xFF, 0x00, 0x00};
uint8_t Centigrade[] = 			{0x07, 0x05, 0x77, 0x88, 0x88, 0x88, 0x88, 0x00};
uint8_t LockPic[] = 				{0x00, 0x78, 0x4C, 0x4A, 0x4A, 0x4C, 0x78, 0x00};
uint8_t UnlockPic[] = 			{0x02, 0x7A, 0x4C, 0x48, 0x48, 0x48, 0x78, 0x00};
uint8_t Arrow_UpDown[] =		{0x00, 0x24, 0x66, 0xFF, 0xFF, 0x66, 0x24, 0x00};
uint8_t Arrow_Left[] =			{0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x00};
uint8_t Arrow_Right[] =			{0x00, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00};

uint8_t Batt_Head_Full[] =  {0x5A, 0x5A, 0x5A, 0x5A, 0x42, 0x7E, 0x3C, 0x00};
uint8_t Batt_Head_Empty[] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x3C, 0x00};
uint8_t Batt_Midd_Full[] =  {0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
uint8_t Batt_Midd_Half[] =  {0x5A, 0x5A, 0x5A, 0x5A, 0x42, 0x42, 0x42, 0x42};
uint8_t Batt_Midd_Empty[] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
uint8_t Batt_Tail_Full[] =  {0x00, 0x7E, 0x42, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
uint8_t Batt_Tail_Empty[] = {0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};

//uint8_t Batt_Usage[] = {
//	0x00,0x7E,0x42,0x42,0x42,0x42,0x42,0x42,
//	0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,
//	0x42,0x42,0x42,0x42,0x42,0x7E,0x3C,0x00
//};

//void BattUsageProgressBarCreate(uint8_t BranchesNum)
//{
//	//5+8+4=17 ProgressBar
//	for(uint8_t i = 0; i < 17; i++)
//	{
//		if(i <= BranchesNum)
//		{
//			Batt_Usage[i+3] |= B00011000;
//		}
//		else
//		{
//			Batt_Usage[i+3] &= B11100111;
//		}
//	}
//}

float BattUsageDisplay()
{
    static float lastBattUsage = 0;
    float BattUsage, BattVoltage = Read_BattVoltage();
    if(BattVoltage < BattEmptyVoltage) BattUsage = 0.0;
    else if(BattVoltage > BattFullVoltage) BattUsage = 1.0;
    else BattUsage = (BattVoltage - BattEmptyVoltage) / (BattFullVoltage - BattEmptyVoltage);
    if(ABS(BattUsage - lastBattUsage) > 0.015)
    {
        lastBattUsage = BattUsage;
    }

//	BattUsageProgressBarCreate(BattUsage*17);
//	oled.drawBitmap(x, y, Batt_Usage, sizeof(Batt_Usage));

    if(lastBattUsage > 0.1)
    {
        oled.drawBitmap(Batt_Tail_Full);
    }
    else
    {
        oled.drawBitmap(Batt_Tail_Empty);
    }

    if(lastBattUsage > 0.75)
    {
        oled.drawBitmap(Batt_Midd_Full);
    }
    else if(lastBattUsage > 0.5)
    {
        oled.drawBitmap(Batt_Midd_Half);
    }
    else
    {
        oled.drawBitmap(Batt_Midd_Empty);
    }

    if(lastBattUsage > 0.85)
    {
        oled.drawBitmap(Batt_Head_Full);
    }
    else
    {
        oled.drawBitmap(Batt_Head_Empty);
    }

    return lastBattUsage;
}

void InfoDisplay(void)
{
    static unsigned long TimePoint;//LCD刷新时间点
    static String NOW_ACT;
    if (millis() >= TimePoint)
    {
        //***********************连接状态、模式显示*************************//
        oled.setCursor(1, 0);
        oled.drawBitmap(ANT);
        if (CTRL_State == On && Get_ConnectionState())
        {
            oled.drawBitmap(ANT_full);
        }
        else
        {
            if (CTRL_State == Off)oled.print('_');
            else oled.print('x');
        }

        oled.setCursor(3, 0);
        if (CTRL.Pattern == CtrlPattern_NRF)oled.print('N');
        else oled.print('U');
        oled.print(CTRL.Mode);
        if (CTRL.Object == CAR)oled.print('c');
        else if (CTRL.Object == TANK)oled.print('t');
        else if (CTRL.Object == CPTR)oled.print('p');
        else if (CTRL.Object == SPIDER)oled.print('s');
        else oled.print('_');

        oled.setCursor(1, 5);
        oled.print("MPU:");
        if(MPU_Switch == On)oled.print("ON ");
        else oled.print("OFF");

        oled.setCursor(10, 5);
        oled.print("SCRN:");
        if(digitalRead(SPDT_Switch_Pin) == On)oled.drawBitmap(LockPic);
        else oled.drawBitmap(UnlockPic);

        oled.setCursor(1, 6);
        oled.printf("Batt:%1.1fV", Read_BattVoltage());
        oled.setCursor(10, 6);
        oled.printf("%3d", (int)(BattUsageDisplay() * 100));
        //BattUseageDisplay();

        oled.setCursor(1, 7);
        oled.printf("LOOP:%5dt/s", MainLoop_Speed);

        //************************其他信息************************//
        if (CTRL.Object == CAR || CTRL.Object == TANK)
        {
            oled.setCursor(6, 0);
            if (Slave_BattVol < 10)oled.print(' ');
            oled.print(Slave_BattVol, 1);
            oled.print('V');

            oled.setCursor(11, 0);
            oled.print(Slave_Temp, 1);
            oled.drawBitmap(Centigrade);
            oled.print(' ');

            if (CTRL.Pattern == CtrlPattern_NRF)
            {
                HMI_Display();
            }
        }
        else if (CTRL.Object == CPTR)
        {
            oled.setCursor(6, 0);
            if (Slave_BattVol < 10)oled.print(' ');
            oled.print(Slave_BattVol, 1);
            oled.print('V');

            oled.setCursor(11, 0);
            //oled.print("CPU:");
            oled.print(Slave_Temp, 1);
            oled.drawBitmap(Centigrade);
            oled.print(' ');

        }
//    else if (CTRL.Object == SPIDER)
//    {
//      oled.setCursor(1, 7);
//      oled.print("SpiserUnsupport");
//    }

        oled.setCursor(1, 1);
        oled.printf("Lp:%4d", L_PWM);
        oled.setCursor(9, 1);
        oled.printf("Rp:%4d", R_PWM);

        oled.setCursor(2, 2);
        oled.printf("x:%4d", JS_L.X);
        oled.setCursor(10, 2);
        oled.printf("x:%4d", JS_R.X);

        oled.setCursor(2, 3);
        oled.printf("y:%4d", JS_L.Y);
        oled.setCursor(10, 3);
        oled.printf("y:%4d", JS_R.Y);

//		oled.setCursor(2, 4);
//		oled.printf("z:%4d",L_CRE);
//		oled.setCursor(10, 4);
//		oled.printf("z:%4d",R_CRE);

        if(CTRL_State == On && Get_ConnectionState())
        {
            if(Ring_Last != 2)BuzzRing(2);
        }
        else
        {
            if(Ring_Last != 1)BuzzRing(1);
        }

        TimePoint = millis() + 50;
    }
}

void OLED_Refresh_Handler()
{
    static uint8_t OLED_Brightness_Last = 0;

    if(MPU_Reading == Off)
    {
        if(OLED_Brightness != OLED_Brightness_Last)
        {
            oled.setBrightness(((float)OLED_Brightness / 100.0) * 255);
            OLED_Brightness_Last = OLED_Brightness;
        }
        oled.putCache();
    }
    EncoderLED_Handler();
}

void OLED_DynamicPrint(uint16_t x, uint16_t y, String Str, uint32_t dly)
{
    byte strleth = Str.length();
    char str[20];
    Str.toCharArray(str, strleth + 1);

    byte i = 0;
    while (i < strleth)
    {
        if (str[i] >= 'A' && str[i] <= 'z')
        {
            for (char j = 'A'; j <= str[i]; j++)
            {
                oled.setCursor(x, y);
                oled.print(j);
                if(Silent_Mode == Off)
                {
                    tonePro(BUZZ_Pin, 7000, dly, 60);
                    delay(dly);
                }
                else delay(dly);
            }
        }
        else if (str[i] >= ' ' && str[i] <= '@')
        {
            for (char j = ' '; j <= str[i]; j++)
            {
                oled.setCursor(x, y);
                oled.print(j);
                if(Silent_Mode == Off)
                {
                    tonePro(BUZZ_Pin, 7000, dly, 60);
                    delay(dly);
                }
                else delay(dly);
            }
        }
        else
        {
            oled.setCursor(x, y);
            oled.print(str[i]);
        }
        x++;
        i++;
    }
}

void AboutInfoPage()
{
    KeyClear();
    oled.clear();
    delayMicroseconds(DisplayFreq_Default * 2);//wait oled clear
    OLED_Display(DISABLE);
    oled.setCacheMode(Off);
    oled.clear();
    oled.drawFullScreen(X_CTRL_LOGO, sizeof(X_CTRL_LOGO));
    OLED_DynamicPrint(2, 6, __DATE__, 0);
    OLED_DynamicPrint(1, 7, String(_X_CTRL_VERSION) + ("_VIFEXTech"), 2);
    while (1)
    {
        Read_Keys();
        EncoderLED_Handler();
        if (Key[k_ok] || Key[k_back])
        {
            KeyClear();
            oled.clear();
            break;
        }
        delay(DisplayFreq_Default / 1000);
    }
    oled.setCacheMode(On);
    OLED_Display(ENABLE);
}
