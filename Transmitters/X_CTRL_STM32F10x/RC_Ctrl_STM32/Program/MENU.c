#include "FileGroup.h"

void MENU(void)//菜单控制函数
{
  static int menu_page = 1, menu_page_flag, series = 1, series_flag, csr = 1, csr_flag;
  static int setting, set_price, mode;

  //*****************************一级页面*****************************//
  if (series == 1)
  {
    switch (menu_page)
    {
      case 1:
        lcd.setCursor(2, 0), lcd.print("Ctrl-");
        if (CTRL_State == On)lcd.print("ON ");
        else lcd.print("OFF");
        lcd.setCursor(2, 1), lcd.print("Inform");
        break;

      case 2:
        lcd.setCursor(2, 0), lcd.print("Options");
        lcd.setCursor(2, 1), lcd.print("Reset ALL");
        break;
    }

    if (k_up)csr--, k_up = 0;//光标控制
    if (k_down)csr++, k_down = 0;
    if (csr > 4)csr = 1;
    else if (csr < 1)csr = 4;

    if (csr > 0 && csr <= 2)menu_page = 1;//页面控制
    else if (csr > 2 && csr <= 4)menu_page = 2;

    if (k_ok == 1)//读取确定键,根据光标的位置来控制相应的标志位
    {
      k_ok = 0;
      if (csr == 1)
      {
        if (CTRL_State == Off)MsTimer2::start(), CTRL_State = On;
        else MsTimer2::stop(), CTRL_State = Off;
      }
      else if (csr == 2)
      {
        series = 2;
        csr_flag = csr;
        csr = 1;
        Menu_Sta = INF;
      }
      else if (csr == 3)
      {
        series = 2;//进入二级页面
        csr_flag = csr;
        csr = 1;//重定位光标
        Menu_Sta = OPT;
      }
      else if (csr == 4)
      {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Comfirm ?");
        while (1)
        {
          KEY_Read();
          if (k_ok)
          {
            k_ok = 0;
            lcd.clear();
            lcd.setCursor(2, 0);
            lcd.print("Wating...");
            DATA('c');
            lcd.clear();
            lcd.setCursor(2, 0);
            lcd.print("Clear DONE!");
            delay(1000);
            lcd.clear();
            break;
          }
          if (k_back)
          {
            k_back = 0;
            lcd.clear();
            break;
          }
        }
      }
    }
    if (k_back)k_back = 0;//不使用后退键
  }

  //*****************************二级页面*****************************//
  else if (series == 2)
  {
    if (Menu_Sta == INF)//信息显示
    {
      LCD_SHOW();
    }

    else if (Menu_Sta == OPT) //设置选项
    {
      switch (menu_page)
      {
        case 1:
          lcd.setCursor(2, 0); lcd.print("Calibration");
          lcd.setCursor(2, 1); lcd.print("Send fq:");
          lcd.print(S_freq);
          lcd.print("ms  ");
          break;

        case 2:
          lcd.setCursor(2, 0), lcd.print("Ctrl MODE:"), lcd.print(ctrl_md);
          lcd.setCursor(2, 1), lcd.print("Ctrl OBJE:");
          if (CTRL_object == CAR)lcd.print("CAR ");
          else if (CTRL_object == TANK)lcd.print("TANK");
          else if (CTRL_object == CPTR)lcd.print("CPTR");
          else if (CTRL_object == SPIDER)lcd.print("SPID");
          break;

        case 3:
          lcd.setCursor(2, 0); lcd.print("Module:");
          if (CTRL_module == nRF)lcd.print("2.4G");
          else lcd.print("UART");
          lcd.setCursor(2, 1), lcd.print("BackLight-");
          if (BL_ON)lcd.print("ON ");
          else lcd.print("OFF");
          break;

        case 4:
          lcd.setCursor(2, 0), lcd.print("Servo SET");
          lcd.setCursor(2, 1), lcd.print("Baud:"), lcd.print(Baud), lcd.print("  ");
          break;

        case 5:
          lcd.setCursor(2, 0), lcd.print("MPU6050");
          if (MPU_ON)lcd.print('*');
          else lcd.print(' ');
          break;
      }


      if (k_down  && setting == 0)csr++, k_down = 0;
      if (k_up  && setting == 0)csr--, k_up = 0;
      if (csr > 9)csr = 1;
      else if (csr < 1)csr = 9;

      if (csr > 0 && csr <= 2)menu_page = 1;//页面控制
      else if (csr > 2 && csr <= 4)menu_page = 2;
      else if (csr > 4 && csr <= 6)menu_page = 3;
      else if (csr > 6 && csr <= 8)menu_page = 4;
      else if (csr > 8 && csr <= 10)menu_page = 5;

      if (setting == 1)//变量调整
      {
        switch (set_price)
        {
          case 2:
            if (k_up)if (S_freq < 5000)S_freq++, k_up = 0;
            if (k_down)if (S_freq > 10)S_freq--, k_down = 0; break;

          case 3:
            if (k_up)if (ctrl_md < 3)ctrl_md++, k_up = 0;
            if (k_down)if (ctrl_md > 1)ctrl_md--, k_down = 0; break;
          //if (ctrl_md > 3)ctrl_md = 1;
          //if (ctrl_md < 1)ctrl_md = 3;

          case 4:
            if (k_up)CTRL_object++, k_up = 0;
            if (k_down)CTRL_object--, k_down = 0;
            Limit(&CTRL_object, CAR, SPIDER);
            break;

          case 8:
            if (k_up)if (Baud < 115200)Baud += 200, k_up = 0;
            if (k_down)if (Baud > 1200)Baud -= 200, k_down = 0; break;
        }
      }
      if (k_ok == 1)
      {
        k_ok = 0;
        switch (csr)
        {
          case 1:
            MsTimer2::stop();
            CTRL_State = Off;
            Calibrat_JSMaxMin();
            break;

          case 5:
            CTRL_module = !CTRL_module;
            DATA('s');
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print("Please Restart");
            delay(2000);
            lcd.clear();
            break;

          case 6:
            if (BL_ON == 1)lcd.noBacklight(), BL_ON = 0;
            else lcd.backlight(), BL_ON = 1;
            break;

          case 7:
            MsTimer2::stop();
            CTRL_State = Off;
            Calibrat_ServoMP();
            break;

          case 9:
            MsTimer2::stop();
            CTRL_State = Off;
            Calibrat_MPU6050();
            break;
        }
        if (setting == 0 && (( csr > 1 && csr < 5) || csr == 8))setting = 1, set_price = csr; //将光标位赋值给相应要设置的变量
      }
    }

    if (k_back)//退出变量设置或退至一级页面
    {
      if (setting)
      {
        setting = 0;
        MsTimer2::set(S_freq, SEND);
        MsTimer2::stop();
        DATA('s');
        CTRL_State = Off;
      }
      else
      {
        series = 1;//退至一级页面
        csr = csr_flag;
        Menu_Sta = OPT;
      }
      k_back = 0;
    }
  }

  //*****************************光标显示*******************************//
  if (csr % 2 == 0 && Menu_Sta == OPT)
  {
    if (setting)
    {
      lcd.setCursor(0, 0), lcd.print("  ");
      lcd.setCursor(0, 1), lcd.print("->");
    }
    else
    {
      lcd.setCursor(0, 0), lcd.print("  ");
      lcd.setCursor(0, 1), lcd.print("> ");
    }
  }
  else if (csr % 2 == 1 && Menu_Sta == OPT)
  {
    if (setting)
    {
      lcd.setCursor(0, 0), lcd.print("->");
      lcd.setCursor(0, 1), lcd.print("  ");
    }
    else
    {
      lcd.setCursor(0, 0), lcd.print("> ");
      lcd.setCursor(0, 1), lcd.print("  ");
    }
  }

  if (menu_page != menu_page_flag || series != series_flag)//自动清屏
  {
    lcd.clear();
    if (menu_page != menu_page_flag)menu_page_flag = menu_page;
    if (series != series_flag)series_flag = series;
  }

  //Serial.println(menu_slt);
}