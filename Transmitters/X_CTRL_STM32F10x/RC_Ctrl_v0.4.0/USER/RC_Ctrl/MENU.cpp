#include "FileGroup.h"

int16_t MenuPage = 1, MenuPage_Last, MenuSeries = 1, MenuSeries_Last, MenuCursor = 1, MenuCursor_Last;
int16_t MenuSetValue, MenuSetValue_Cursor;

void MenuRun()//菜单控制函数
{
    MenuCursorToPage();
    if (MenuSeries == 1) BasicPage();
    else if (MenuSeries == 2)
    {
        if (MenuState == INF)LCD_Display();//信息显示
        else if (MenuState == OPT) OptionsPage();//设置选项

        if (Key[k_back])//退出变量设置或退至一级页面
        {
            if (MenuSetValue == On)
            {
                MenuSetValue = Off;
                MsTimer2_set(SendFreq, SendData);
                MsTimer2_stop();
                DataStorageHandle(SaveData);
                CTRL_State = Off;
            }
            else
            {
                MenuSeries = 1;//退至一级页面
                MenuCursor = MenuCursor_Last;
                DataStorageHandle(SaveData);
                MenuState = OPT;
            }
            Key[k_back] = 0;
        }
    }

    if (MenuState == OPT)ShowMenuCursor(); //光标显示
    MenuAutoClear();
}


void BasicPage()
{
    switch (MenuPage)
    {
    case 1:
        lcd.setCursor(2, 0), lcd.print(F("Ctrl-"));
        if (CTRL_State == On)lcd.print(F("ON "));
        else lcd.print(F("OFF"));
        lcd.setCursor(2, 1), lcd.print(F("Inform"));
        break;

    case 2:
        lcd.setCursor(2, 0), lcd.print(F("Options"));
        lcd.setCursor(2, 1), lcd.print(F("About"));
        break;
    }

    if (Key[k_up])MenuCursor--, Key[k_up] = 0;//光标控制
    if (Key[k_down])MenuCursor++, Key[k_down] = 0;
    if (MenuCursor > 4)MenuCursor = 1;
    else if (MenuCursor < 1)MenuCursor = 4;

    if (Key[k_ok] == 1)//读取确定键,根据光标的位置来控制相应的标志位
    {
        Key[k_ok] = 0;
        if (MenuCursor == 1)
        {
            if (CTRL_State == Off)MsTimer2_start(), CTRL_State = On;
            else MsTimer2_stop(), CTRL_State = Off;
        }
        else if (MenuCursor == 2)
        {
            MenuSeries = 2;
            MenuCursor_Last = MenuCursor;
            MenuCursor = 1;
            MenuState = INF;
        }
        else if (MenuCursor == 3)
        {
            MenuSeries = 2;//进入二级页面
            MenuCursor_Last = MenuCursor;
            MenuCursor = 1;//重定位光标
            MenuState = OPT;
        }
        else if (MenuCursor == 4)
        {
            lcd.clear();
			LCD_DynamicPrint(2,0,(char*)__DATE__,5);
			LCD_DynamicPrint(1,1,(char*)_VERSION,5);
			LCD_DynamicPrint(5,1,(char*)"_VIFEXTech",5);
            while (1)
            {
                Read_Keys();
				
                if (Key[k_ok] || Key[k_back])
                {
                    Key[k_ok] = 0;
                    Key[k_back] = 0;
                    lcd.clear();
                    break;
                }
            }
        }
    }
    if (Key[k_back])Key[k_back] = 0;//不使用后退键
}

void OptionsPage()
{
    switch (MenuPage)
    {
    case 1:
        lcd.setCursor(2, 0);
        lcd.print(F("JoyStick"));
        lcd.setCursor(2, 1);
        lcd.print(F("Send fq:"));
        lcd.print(SendFreq);
        lcd.print(F("ms  "));
        break;

    case 2:
        lcd.setCursor(2, 0), lcd.print(F("Mode:")), lcd.print(CtrlMode);
        lcd.setCursor(9, 0);
        switch (CtrlMode)
        {
        case 1:
            lcd.write(3), lcd_Space(2), lcd.write(3), lcd_Space(1);
            break;

        case 2:
            lcd.write(3), lcd_Space(2), lcd.write(4), lcd.write(5);
            break;

        case 3:
            lcd.write(4), lcd.write(3), lcd.write(5), lcd.print(F(" S"));
            break;
        }

        lcd.setCursor(2, 1), lcd.print(F("Object:"));
        if (CTRL_Object == CAR)lcd.print(F("CAR "));
        else if (CTRL_Object == TANK)lcd.print(F("TANK"));
        else if (CTRL_Object == CPTR)lcd.print(F("CPTR"));
        else if (CTRL_Object == SPIDER)lcd.print(F("SPID"));
        else if (CTRL_Object == CRAWLER)lcd.print(F("CRAW"));
        break;

    case 3:
        lcd.setCursor(2, 0);
        lcd.print(F("Module:"));
        if (CTRL_Module == Module_NRF)lcd.print(F("2.4G"));
        else lcd.print(F("UART"));
        lcd.setCursor(2, 1), lcd.print(F("BackLight-"));
        if (BackLight_Switch == On)lcd.print(F("ON "));
        else lcd.print(F("OFF"));
        break;

    case 4:
        lcd.setCursor(2, 0), lcd.print(F("Servo"));
        lcd.setCursor(2, 1), lcd.print(F("Baud:")), lcd.print(UART_Baud[UART_Baud_Select]), lcd_Space(2);
        break;

    case 5:
        lcd.setCursor(2, 0), lcd.print(F("MPU6050"));
        if (MPU_Switch == On)lcd.print('*');
        else lcd.print(' ');
        lcd.setCursor(2, 1), lcd.print(F("BT Setup"));
        break;

    case 6:
        lcd.setCursor(2, 0), lcd.print(F("Reset ALL"));
        break;
    }


    if (Key[k_down]  && MenuSetValue == Off)MenuCursor++, Key[k_down] = 0;
    if (Key[k_up]  && MenuSetValue == Off)MenuCursor--, Key[k_up] = 0;
    if (MenuCursor > 11)MenuCursor = 1;
    else if (MenuCursor < 1)MenuCursor = 11;

    if (Key[k_ok] && MenuSetValue == Off && (( MenuCursor > 1 && MenuCursor < 5) || MenuCursor == 8))
    {
        Key[k_ok] = 0;
        MenuSetValue = On;
        MenuSetValue_Cursor = MenuCursor; //将光标位赋值给相应要设置的变量
    }
    else if (Key[k_ok] && MenuSetValue == Off)
    {
        Key[k_ok] = 0;
        switch (MenuCursor)
        {
        case 1:
            MsTimer2_stop();
            CTRL_State = Off;
            Calibrat_JSMaxMin();
            break;

        case 5:
            CTRL_Module = !CTRL_Module;
            DataStorageHandle(SaveData);
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print(F("Please Restart"));
            delay(2000);
            lcd.clear();
            break;

        case 6:
            if (BackLight_Switch == On)lcd.noBacklight(), BackLight_Switch = Off;
            else lcd.backlight(), BackLight_Switch = On;
            break;

        case 7:
            MsTimer2_stop();
            CTRL_State = Off;
            Calibrat_ServoMP(0, 180, 0, 180);
            break;

        case 9:
            MsTimer2_stop();
            CTRL_State = Off;

#ifdef USE_MPU6050
            Calibrat_MPU6050();
#endif
            break;

        case 10:
            MsTimer2_stop();
            CTRL_State = Off;
            BT_Setup();
            break;

        case 11:
            MsTimer2_stop();
            CTRL_State = Off;
            ClearAllData();
            break;
        }
    }

    if (MenuSetValue == On)//变量调整
    {
        switch (MenuSetValue_Cursor)
        {
        case 2:
            if (Key[k_up])if (SendFreq < 5000)SendFreq++, Key[k_up] = 0;
            if (Key[k_down])if (SendFreq > 10)SendFreq--, Key[k_down] = 0;
            break;

        case 3:
            if (Key[k_up])if (CtrlMode < 3)CtrlMode++, Key[k_up] = 0;
            if (Key[k_down])if (CtrlMode > 1)CtrlMode--, Key[k_down] = 0;
            break;

        case 4:
            if (Key[k_up])CTRL_Object++, Key[k_up] = 0;
            if (Key[k_down])CTRL_Object--, Key[k_down] = 0;
            Limit(CTRL_Object, CAR, CRAWLER);
            break;

        case 8:
            if (Key[k_up])if (UART_Baud_Select < 10)UART_Baud_Select ++, Key[k_up] = 0;
            if (Key[k_down])if (UART_Baud_Select > 0)UART_Baud_Select --, Key[k_down] = 0;
            break;
        }
    }
}

void ShowMenuCursor()
{
    for (uint8_t i = 0; i < 2; i++ )
    {
        lcd.setCursor(0, i);
        if (i == (MenuCursor - 1) % 2)
        {
            if (MenuSetValue == On) lcd.print(F("->"));
            else lcd.print(F("> "));
        }
        else lcd_Space(2);
    }
}

void NumClear(uint32_t Num, int16_t digit)
{
    if (digit <= 1)return;
    uint32_t HideNum = powf(10, digit - 1);
    while (HideNum >= 10)
    {
        if (Num < HideNum)lcd.print(' ');
        HideNum /= 10;
    }
}

void MenuAutoClear()//自动清屏
{
    if (MenuPage != MenuPage_Last || MenuSeries != MenuSeries_Last)
    {
        lcd.clear();
        if (MenuPage != MenuPage_Last)MenuPage_Last = MenuPage;
        if (MenuSeries != MenuSeries_Last)MenuSeries_Last = MenuSeries;
    }
}

void MenuCursorToPage()
{
    if (MenuCursor > 0 && MenuCursor <= 2)MenuPage = 1;//页面控制
    else if (MenuCursor > 2 && MenuCursor <= 4)MenuPage = 2;
    else if (MenuCursor > 4 && MenuCursor <= 6)MenuPage = 3;
    else if (MenuCursor > 6 && MenuCursor <= 8)MenuPage = 4;
    else if (MenuCursor > 8 && MenuCursor <= 10)MenuPage = 5;
    else if (MenuCursor > 10 && MenuCursor <= 12)MenuPage = 6;
}

