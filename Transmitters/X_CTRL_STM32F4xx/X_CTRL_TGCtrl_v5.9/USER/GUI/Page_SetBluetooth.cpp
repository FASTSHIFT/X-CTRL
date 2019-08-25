#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*蓝牙使能控制*/
bool State_Bluetooth = ON;

/*蓝牙连接对象*/
uint8_t Bluetooth_ConnectObject = BC_Type::BC_PHE;

/*实例化HC05蓝牙对象*/
Bluetooth_HC05 hc05(&Serial2, BT_State_Pin, BT_EN_Pin, BT_PowerCtrl_Pin);

/*蓝牙可选波特率*/
static const uint32_t UseBaudRate[] = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 43000, 57600, 76800, 115200};

/*蓝牙可选波特率个数*/
#define UseBaudRate_Size __Sizeof(UseBaudRate)

/*蓝牙波特率选择项*/
static uint8_t BaudRateSelect = 0;

/*密码*/
static String Password;

/*主从角色*/
static bool Role;

/*选项最大个数*/
#define ItemSelect_MAX 4

/*选项字符串缓冲*/
static String ItemStr[ItemSelect_MAX];

/*当前选中的选项*/
static int16_t ItemSelect = 0;

/*选项更新标志位*/
static bool ItemSelectUpdating = false;

/*选项显示起始坐标*/
#define ItemStartY (StatusBar_POS+8)
#define ItemStartX 14


/**
  * @brief  更新选项字符串
  * @param  无
  * @retval 无
  */
static void UpdateItemStr()
{
    for(uint8_t i = 0; i < ItemSelect_MAX; i++)
    {
        if(i == ItemSelect)
            screen.setTextColor(screen.Yellow, screen.Black);
        else
            screen.setTextColor(screen.White, screen.Black);
        
        screen.setCursor(ItemStartX, ItemStartY + i * 10);
        screen.print(ItemStr[i]);
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(20, StatusBar_POS + 10);
    screen.print("Enter AT Mode...");
    screen.setCursor(20, StatusBar_POS + 20);
    if(hc05.AT_Enter())//进入AT模式
    {
        ItemSelect = 0;
        BaudRateSelect = 0;
        ItemSelectUpdating = false;
        screen.setTextColor(screen.Green, screen.Black);
        screen.print("SUCCESS");
        
        uint32_t baudRate = 0;
        String name,sbaudRate,password;
        
        if(!hc05.GetName(&name)) 
            name = "*";
        
        if(!hc05.GetBaudRate(&baudRate))
            sbaudRate="*";
        else
            sbaudRate = String(baudRate);
            
        if(!hc05.GetPassword(&password))
            password = "*";
        
        ItemStr[0] = "1.Name:" + name;
        ItemStr[1] = "2.BaudRate:" + sbaudRate;
        ItemStr[2] = "3.Password:" + password;
        ItemStr[3] = "4.Role:" + String(hc05.GetRole() ? "Master" : "Slaver");
        Role = hc05.GetRole();
        ClearPage();
        UpdateItemStr();
    }
    else
    {
        screen.setTextColor(screen.Red, screen.Black);
        screen.print("FAILED");
        PageDelay(1000);
        page.PageChangeTo(PAGE_Options);
    }
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    if(ItemSelectUpdating)
    {
        UpdateItemStr();
        ItemSelectUpdating = false;
    }
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    ClearPage();
    screen.setTextColor(screen.White, screen.Black);
    screen.setCursor(20, StatusBar_POS + 10);
    screen.print("Exit AT Mode...");
    hc05.SetName(String(Master_Description));
    hc05.SetBaudRate(UseBaudRate[BaudRateSelect]);
    hc05.SetPassword(Password);
    hc05.SetRole(Role);
    hc05.AT_Exit();
    Serial.begin(UseBaudRate[BaudRateSelect]);
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    if(btOK || btEcd)
    {
        switch(ItemSelect)
        {
        case 1:
            BaudRateSelect = (BaudRateSelect + 1) % UseBaudRate_Size;
            ItemStr[1] = "2.BaudRate:" + String(UseBaudRate[BaudRateSelect]) + "  ";
            ItemSelectUpdating = true;
            break;
        case 2:
            static uint8_t i = 0;
            i++;
            i %= 10;
            Password = String(i * 1000 + i * 100 + i * 10 + i);
            ItemStr[2] = "3.Password:" + Password;
            ItemSelectUpdating = true;
            break;
        case 3:
            Role = ! Role;
            ItemStr[3] = "4.Role:" + String(Role ? "Master" : "Slaver");
            ItemSelectUpdating = true;
            break;
        }
    }
    if(btBACK)
    {
        page.PageChangeTo(PAGE_Options);
    }

    if(btUP)
    {
        ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
    if(btDOWN)
    {
        ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
}

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
static void EncoderEvent()
{
    if(ecd > 0)
    {
        ItemSelect = (ItemSelect + 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
    if(ecd < 0)
    {
        ItemSelect = (ItemSelect + ItemSelect_MAX - 1) % ItemSelect_MAX;
        ItemSelectUpdating = true;
    }
}

/**
  * @brief  蓝牙设置页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_SetBluetooth(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
