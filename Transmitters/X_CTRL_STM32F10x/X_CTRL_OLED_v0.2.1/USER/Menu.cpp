#include "FileGroup.h"

uint8_t Silent_Mode = Off;
uint8_t MenuState = OPT;
uint8_t CTRL_State = Off;
uint8_t OLED_Brightness = 100;

int MenuPage = 1, MenuSeries = 1, MenuCursor = 1, MenuCursor_Last;
int MenuSetValue, MenuSetValue_Cursor;

void ShowMenuCursor()
{
    for(uint8_t i = 0; i < 8; i++ )
    {
        oled.setCursor(0, i);
        if(i == (MenuCursor - 1) % 8)
        {
            oled.drawBitmap(Triangle);
            if(MenuSetValue) oled.drawBitmap(Triangle_UD);
            else oled.print(' ');
        }
        else oled.print("  ");
    }
}

void MenuAutoClear()
{
    static int MenuPage_Last, MenuSeries_Last;
    if (MenuPage != MenuPage_Last || MenuSeries != MenuSeries_Last)//自动清屏
    {
        oled.clear();
        if (MenuPage != MenuPage_Last)MenuPage_Last = MenuPage;
        if (MenuSeries != MenuSeries_Last)MenuSeries_Last = MenuSeries;
    }
}

void NumClear(unsigned long Num, int digit)
{
    if(digit <= 1)return;
    unsigned long HideNum = powf(10, digit - 1);
    while(HideNum >= 10)
    {
        if(Num < HideNum)oled.print(' ');
        HideNum /= 10;
    }
}

void BasicPage()
{
    switch (MenuPage)
    {
    case 1:
        oled.setCursor(2, 0), oled.print("Ctrl-");
        if (CTRL_State == On)oled.print("ON ");
        else oled.print("OFF");
        oled.setCursor(2, 1), oled.print("Inform");
        oled.setCursor(2, 2), oled.print("Options");
        oled.setCursor(2, 3), oled.print("About");
        break;
    }

    if (Key[k_up])MenuCursor--;//光标控制
    if (Key[k_down])MenuCursor++;
    if (MenuCursor > 4)MenuCursor = 1;
    else if (MenuCursor < 1)MenuCursor = 4;

    if (MenuCursor > 0 && MenuCursor <= 8)MenuPage = 1;//页面控制
    else if (MenuCursor > 8 && MenuCursor <= 16)MenuPage = 2;

    if (Key[k_ok])//读取确定键,根据光标的位置来控制相应的标志位
    {
        if (MenuCursor == 1)
        {
            if (CTRL_State == Off)
            {
                SendData_Switch(ENABLE);
                CTRL_State = On;
            }
            else
            {
                SendData_Switch(DISABLE);
                CTRL_State = Off;
            }
        }
        else if (MenuCursor == 2)
        {
            MenuSeries = 2;
            MenuCursor_Last = MenuCursor;
            MenuCursor = 1;
            MenuState = INF;
            SendData_Switch(ENABLE);
            CTRL_State = On;
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
            AboutInfoPage();
        }
    }
}

void OptionsPage()
{
    switch (MenuPage)
    {
    case 1:
        oled.setCursor(2, 0), oled.print("Joystick");
        oled.setCursor(2, 1), oled.printf("Send fq:%03dms", CTRL.SendFreq);
        oled.setCursor(2, 2), oled.printf("Mode:%d", CTRL.Mode);
        oled.setCursor(9, 2);
        switch (CTRL.Mode)
        {
        case 1:
            oled.drawBitmap(Arrow_UpDown);
            oled.print("  ");
            oled.drawBitmap(Arrow_UpDown);
            oled.print(' ');
            break;

        case 2:
            oled.drawBitmap(Arrow_UpDown);
            oled.print("  ");
            oled.drawBitmap(Arrow_Left);
            oled.drawBitmap(Arrow_Right);
            break;

        case 3:
            oled.drawBitmap(Arrow_Left);
            oled.drawBitmap(Arrow_UpDown);
            oled.drawBitmap(Arrow_Right);
            oled.print(" S");
            break;
        }

        oled.setCursor(2, 3), oled.print("Object:");
        if (CTRL.Object == CAR)oled.print("CAR ");
        else if (CTRL.Object == TANK)oled.print("TANK");
        else if (CTRL.Object == CPTR)oled.print("CPTR");
        else if (CTRL.Object == SPIDER)oled.print("SPID");
        else if (CTRL.Object == CRAWLER)oled.print("CRAW");

        oled.setCursor(2, 4), oled.print("Pattern:");
        if (CTRL.Pattern == CtrlPattern_NRF)oled.print("2.4G");
        else oled.print("UART");
        oled.setCursor(2, 5), oled.printf("Bright:%3d%%", OLED_Brightness);
        oled.setCursor(2, 6), oled.print("Servo");
        oled.setCursor(2, 7), oled.printf("Baud:%6d", UART_Baud[CTRL.Baud_Select]);
        break;

    case 2:
        oled.setCursor(2, 0), oled.print("MPU6050-");
        if (MPU_Switch == On)oled.print("ON ");
        else oled.print("OFF");
        oled.setCursor(2, 1), oled.print("BT Setup");
        oled.setCursor(2, 2), oled.print("Reset ALL");
        break;
    }


    if (Key[k_down]  && MenuSetValue == 0)MenuCursor++;
    if (Key[k_up]  && MenuSetValue == 0)MenuCursor--;
    if (MenuCursor > 11)MenuCursor = 1;
    else if (MenuCursor < 1)MenuCursor = 11;

    if (MenuCursor > 0 && MenuCursor <= 8)MenuPage = 1;//页面控制
    else if (MenuCursor > 8 && MenuCursor <= 16)MenuPage = 2;

    if (Key[k_ok] && MenuSetValue == 0 && (( MenuCursor > 1 && MenuCursor <= 4) || MenuCursor == 6 || MenuCursor == 8))
    {
        MenuSetValue = 1;
        MenuSetValue_Cursor = MenuCursor; //将光标位赋值给相应要设置的变量
    }
    else if (Key[k_ok] && MenuSetValue == 0)
    {
        switch (MenuCursor)
        {
        case 1:
            SendData_Switch(ENABLE);
            CTRL_State = Off;
            Calibrat_JSMaxMin();
            break;

        case 5:
            CTRL.Pattern = !CTRL.Pattern;
            DataStorageHandle(SaveData);
            if(CTRL.Pattern == CtrlPattern_NRF)
            {
                Init_NRF();
            }
            break;

        case 7:
            SendData_Switch(DISABLE);
            CTRL_State = Off;
            KeyClear();
            Calibrat_ServoMP(0, 180, 0, 180);
            break;

        case 9:
            SendData_Switch(DISABLE);
            CTRL_State = Off;
            KeyClear();
            Calibrat_MPU6050();
            break;

        case 10:
            SendData_Switch(DISABLE);
            CTRL_State = Off;
            KeyClear();
            BT_Setup();
            break;

        case 11:
            SendData_Switch(DISABLE);
            CTRL_State = Off;
            KeyClear();
            CTRL_SetDefault();
            break;
        }
    }

    if (MenuSetValue)//变量调整
    {
        switch (MenuSetValue_Cursor)
        {
        case 2:
            if (Key[k_up] && CTRL.SendFreq < 999)CTRL.SendFreq++;
            if (Key[k_down] && CTRL.SendFreq > 3)CTRL.SendFreq--;
            break;

        case 3:
            if (Key[k_up] && CTRL.Mode < 3)CTRL.Mode++;
            if (Key[k_down] && CTRL.Mode > 1)CTRL.Mode--;
            break;

        case 4:
            if (Key[k_up] && CTRL.Object < CRAWLER)CTRL.Object++;
            if (Key[k_down] && CTRL.Object > 0)CTRL.Object--;
            break;

        case 6:
            if (Key[k_up] && OLED_Brightness < 100)OLED_Brightness++;
            if (Key[k_down] && OLED_Brightness > 0)OLED_Brightness--;
            break;

        case 8:
            if (Key[k_up] && CTRL.Baud_Select < 10)CTRL.Baud_Select ++;
            if (Key[k_down] && CTRL.Baud_Select > 0)CTRL.Baud_Select --;
            break;
        }
    }
}

void MenuRun(void)//菜单控制函数
{
    //*****************************一级页面*****************************//
    if (MenuSeries == 1)BasicPage();

    //*****************************二级页面*****************************//
    else if (MenuSeries == 2)
    {
        if (MenuState == INF) InfoDisplay();//信息显示
        else if (MenuState == OPT) OptionsPage();//设置选项
        if (Key[k_back])//退出变量设置或退至一级页面
        {
            if (MenuSetValue)
            {
                MenuSetValue = 0;
                TimerSet(TIM2, CTRL.SendFreq * 1000, SendData);
                SendData_Switch(DISABLE);
                DataStorageHandle(SaveData);
                CTRL_State = Off;
            }
            else
            {
                if(MenuState == OPT)
                {
                    MenuSeries = 1;//退至一级页面
                    MenuCursor = MenuCursor_Last;
                    DataStorageHandle(SaveData);
                    MenuState = OPT;
                }
                else if(MenuState == INF && SPDT_State == Off)
                {
                    MenuSeries = 1;//退至一级页面
                    MenuCursor = MenuCursor_Last;
                    MenuState = OPT;
                    KeyClear();
                    TX_DataLoad();//将按键状态清零,再加载一次数据包
                }
            }
        }
    }

    if(MenuState == OPT)ShowMenuCursor();
    MenuAutoClear();
}
