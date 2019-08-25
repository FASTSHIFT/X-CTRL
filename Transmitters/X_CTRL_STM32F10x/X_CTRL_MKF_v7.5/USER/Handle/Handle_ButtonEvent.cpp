#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化按键对象*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;

static ButtonEvent swSPDT;

const uint8_t KEY_Y[] = {KEY_Y1_Pin, KEY_Y2_Pin, KEY_Y3_Pin};
const uint8_t KEY_X[] = {KEY_X1_Pin, KEY_X2_Pin, KEY_X3_Pin};
ButtonEvent btMK[9];

/**
  * @brief  读取矩阵键盘上的一个按键
  * @param  keyID:键ID号
  * @retval 按键状态
  */
bool ReadMatrixKeys(uint8_t keyID)
{
    uint8_t ky = keyID / sizeof(KEY_Y);
    uint8_t kx = keyID % sizeof(KEY_X);
    for(uint8_t y = 0; y < sizeof(KEY_Y); y++)
    {
        (y == ky) ? digitalWrite(KEY_Y[y], HIGH) : digitalWrite(KEY_Y[y], LOW);
    }
    return digitalRead(KEY_X[kx]);
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void When_ButtonPress_Page()
{
    BuzzTone(500, 20);//播放操作音(500Hz, 持续20ms)

    /*置位对应的CTRL按键标志位*/
    if (btUP == ButtonEvent_Type::Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == ButtonEvent_Type::Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == ButtonEvent_Type::Press)
        CTRL.Key |= BT_OK;
    if (btBACK == ButtonEvent_Type::Press)
        CTRL.Key |= BT_BACK;

    page.PageEventTransmit(EVENT_ButtonPress);//向页面调度器发送一个按键按下事件
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
void When_ButtonLongPress_Page()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);//向页面调度器发送一个按键长摁事件
}

/**
  * @brief  按键释放事件
  * @param  无
  * @retval 无
  */
void When_ButtonRelease_Page()
{
    BuzzTone(700, 20);//播放操作音(700Hz, 持续20ms)

    /*清除对应的CTRL按键标志位*/
    if (btUP == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_BACK;
    
    page.PageEventTransmit(EVENT_ButtonRelease);
}

/**
  * @brief  开关状态改变事件
  * @param  无
  * @retval 无
  */
void When_ChangeSPDT()
{
    if(digitalRead(SPDT_Switch_Pin))//判断开关状态
    {
        BuzzTone(500, 300); //播放操作音(500Hz, 持续300ms)
        XFS_Clear();        //清除语音缓存队列
        XFS_Speak("遥控器解锁");//加入语音缓存队列
    }
    else
    {
        BuzzTone(200, 100); //播放操作音(200Hz, 持续100ms)
        XFS_Clear();        //清除语音缓存队列
        XFS_Speak("遥控器锁定");//加入语音缓存队列
    }
}

/**
  * @brief  初始化按键事件
  * @param  无
  * @retval 无
  */
void Init_BottonEvent()
{
    /*开关引脚初始化*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);
    
    /*按键初始化*/
    pinMode(KEY_Y1_Pin, OUTPUT);
    pinMode(KEY_Y2_Pin, OUTPUT);
    pinMode(KEY_Y3_Pin, OUTPUT);
    pinMode(KEY_X1_Pin, INPUT_PULLDOWN);
    pinMode(KEY_X2_Pin, INPUT_PULLDOWN);
    pinMode(KEY_X3_Pin, INPUT_PULLDOWN);

#ifdef USE_Encoder
    /*旋转编码器按键引脚初始化*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);
#endif

    /*按键按下事件关联*/
    btUP.	EventAttach_Press(When_ButtonPress_Page);
    btDOWN.	EventAttach_Press(When_ButtonPress_Page);
    btOK.	EventAttach_Press(When_ButtonPress_Page);
    btBACK.	EventAttach_Press(When_ButtonPress_Page);
    btEcd.	EventAttach_Press(When_ButtonPress_Page);  

    /*按键长摁事件关联*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btOK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress_Page);

    
   /*按键释放事件关联*/
    btUP.	EventAttach_Release(When_ButtonRelease_Page);
    btDOWN.	EventAttach_Release(When_ButtonRelease_Page);
    btOK.	EventAttach_Release(When_ButtonRelease_Page);
    btBACK.	EventAttach_Release(When_ButtonRelease_Page);
    btEcd.	EventAttach_Release(When_ButtonRelease_Page);
    
    /*开关状态改变事件关联*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
    
//    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
//    {
//        btMK[k].EventAttach_Press(When_ButtonPress_Page);
//        btMK[k].EventAttach_LongPress(When_ButtonLongPress_Page);
//        btMK[k].EventAttach_Release(When_ButtonRelease_Page);
//    }
}

/**
  * @brief  按键事件监视
  * @param  无
  * @retval 无
  */
void ButtonEventMonitor()
{
    enum{
        L_Left = 7,
        L_Right = 6,
        L_Up = 3,
        L_Down = 4,
        R_Left = 2,
        R_Right = 5,
        R_Up = 0,
        R_Down = 1
    };
    /*按键事件监视*/
    btUP.   EventMonitor(ReadMatrixKeys(R_Left));
    btDOWN. EventMonitor(ReadMatrixKeys(R_Right));
    btOK.   EventMonitor(ReadMatrixKeys(L_Right));
    btBACK. EventMonitor(ReadMatrixKeys(L_Left));
    
    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
    
    for(uint8_t k = 0; k < __Sizeof(btMK); k++)
    {
        btMK[k].EventMonitor(ReadMatrixKeys(k));
    }
}
